-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2024 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Radim Dvorak <xdvorar00 AT stud.fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_INV  : out std_logic;                      -- pozadavek na aktivaci inverzniho zobrazeni (1)
   OUT_WE   : out std_logic;                      -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'

   -- stavove signaly
   READY    : out std_logic;                      -- hodnota 1 znamena, ze byl procesor inicializovan a zacina vykonavat program
   DONE     : out std_logic                       -- hodnota 1 znamena, ze procesor ukoncil vykonavani programu (narazil na instrukci halt)
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

  signal ptr : std_logic_vector(12 downto 0);
  signal ptr_inc : std_logic;
  signal ptr_dec : std_logic;

  signal pc : std_logic_vector(12 downto 0);
  signal pc_inc : std_logic;
  signal pc_dec : std_logic;

  signal cnt : std_logic_vector(7 downto 0);
  signal cnt_inc : std_logic;
  signal cnt_dec : std_logic;

  signal tmp : std_logic_vector(7 downto 0);
  signal tmp_load : std_logic;

  signal mx1_select : std_logic;
  signal mx2_select : std_logic_vector(1 downto 0);

  type fsm_state is (
    S_START,
    S_INIT,

    S_FETCH,
    S_DECODE,
    
    S_PTR_INC,
    S_PTR_DEC,

    S_VAL_INC_1,
    S_VAL_INC_2,
    S_VAL_INC_3,

    S_VAL_DEC_1,
    S_VAL_DEC_2,
    S_VAL_DEC_3,

    S_WHILE_LEFT,
    S_WHILE_RIGHT_SKIP_1,
    S_WHILE_LEFT_SKIP,
    S_WHILE_LEFT_RBRACKET,

    S_WHILE_RIGHT,
    S_WHILE_RIGHT_SKIP,
    S_WHILE_RIGHT_LBRACKET,

    S_INPUT_1,
    S_INPUT_2,

    S_PRINT_1,
    S_PRINT_2,

    S_TMP_PUT,
    S_TMP_GET,

    S_HALT,

    S_IDLE
  );

  signal current_state : fsm_state;
  signal next_state : fsm_state;

begin

  ptr_reg: process(CLK,RESET,ptr_inc,ptr_dec)
  begin
    if RESET = '1' then
      ptr <= "0000000000000";
    elsif rising_edge(CLK) then
      if ptr_inc = '1' and ptr = "1111111111111" then
        ptr <= "0000000000000";
      elsif ptr_inc = '1' then
        ptr <= ptr + 1;
      elsif ptr_dec = '1' and ptr = "0000000000000" then
        ptr <= "1111111111111";
      elsif ptr_dec = '1' then
        ptr <= ptr - 1;
      end if;
    end if;
  end process;

  pc_reg: process(CLK,RESET,pc_inc,pc_dec)
  begin
    if RESET = '1' then
      pc <= "0000000000000";
    elsif rising_edge(CLK) then
      if pc_inc = '1' and pc = "1111111111111" then
        pc <= "0000000000000";
      elsif pc_inc = '1' then
        pc <= pc + 1;
      elsif pc_dec = '1' and pc = "0000000000000" then
        pc <= "1111111111111";
      elsif pc_dec = '1' then
        pc <= pc - 1;
      end if;
    end if;
  end process;

  tmp_reg : process(CLK,RESET,tmp_load)
  begin
    if RESET = '1' then
      tmp <= x"00";
    elsif rising_edge(CLK) then
      if tmp_load = '1' then
        tmp <= DATA_RDATA;
      end if;
    end if;
  end process;

  cnt_reg : process(CLK,RESET,cnt_inc,cnt_dec)
  begin
    if RESET = '1' then
      cnt <= "00000000";
    elsif rising_edge(CLK) then
      if cnt_inc = '1' then
        cnt <= cnt + 1;
      elsif cnt_dec = '1' then
        cnt <= cnt - 1;
      end if;
    end if;
  end process;

  mx1 : process(mx1_select,ptr,pc)
  begin
    case mx1_select is
      when '0' => DATA_ADDR <= ptr;
      when '1' => DATA_ADDR <= pc;
      when others =>
    end case;
  end process;

  mx2 : process(mx2_select,IN_DATA,DATA_RDATA)
  begin
    case mx2_select is
      when "00" => DATA_WDATA <= IN_DATA;
      when "01" => DATA_WDATA <= tmp;
      when "10" => DATA_WDATA <= DATA_RDATA - 1;
      when "11" => DATA_WDATA <= DATA_RDATA + 1;
      when others =>
    end case;
  end process;

  state_register : process(RESET, CLK, EN) is
  begin
    if RESET = '1' then
      current_state <= S_START;
    elsif rising_edge(CLK) and EN = '1' then
      current_state <= next_state;
    end if;
  end process;

  fsm_next : process(IN_VLD, OUT_BUSY, DATA_RDATA, current_state)
  begin
    ptr_dec <= '0';
    ptr_inc <= '0';

    pc_dec <= '0';
    pc_inc <= '0';

    cnt_dec <= '0';
    cnt_inc <= '0';

    tmp_load <= '0';

    case current_state is
      when S_START =>
        IN_REQ <= '0';
        OUT_WE <= '0';
        DONE <= '0';
        READY <= '0';

        DATA_RDWR <= '1';
        DATA_EN <= '1';

        mx1_select <= '0';
        mx2_select <= "00";
        next_state <= S_INIT;

      when S_INIT =>
        if DATA_RDATA = x"40" then
          READY <= '1';
          next_state <= S_FETCH;
        else
          mx1_select <= '0';
          ptr_inc <= '1';
          next_state <= S_INIT;
        end if;

      when S_FETCH =>
        mx1_select <= '1';
        DATA_RDWR <= '1';
        DATA_EN <= '1';
        next_state <= S_DECODE;
        
      when S_DECODE =>
        case DATA_RDATA is
          -- increment ptr
          when x"3E" =>
            ptr_inc <= '1'; 
            next_state <= S_PTR_INC;
          -- decrement ptr
          when x"3C" => 
            ptr_dec <= '1'; 
            next_state <= S_PTR_DEC;
          -- increment value
          when x"2B" =>
            mx1_select <= '0';
            DATA_RDWR <= '1';
            next_state <= S_VAL_INC_1;
          -- decrement value
          when x"2D" =>
            mx1_select <= '0';
            DATA_RDWR <= '1';
            next_state <= S_VAL_DEC_1;
          -- put value into tmp
          when x"24" =>
            mx1_select <= '0';
            DATA_RDWR <= '1';
            next_state <= S_TMP_PUT;
          -- left bracket
          when x"5B" =>
            mx1_select <= '0';
            DATA_RDWR <= '1';
            pc_inc <= '1';
            next_state <= S_WHILE_LEFT;
          -- right bracket
          when x"5D" =>
            mx1_select <= '0';
            DATA_RDWR <= '1';
            next_state <= S_WHILE_RIGHT;
          -- get value from tmp
          when x"21" => 
            mx1_select <= '0';
            mx2_select <= "01";
            DATA_RDWR <= '0';
            next_state <= S_TMP_GET;
          -- print value
          when x"2E" =>
            OUT_INV <= '0';
            mx1_select <= '0';
            DATA_RDWR <= '1';
            next_state <= S_PRINT_1;
          when x"2C" =>
            IN_REQ <= '1';
            mx1_select <= '0';
            DATA_RDWR <= '1';
            next_state <= S_INPUT_1;
          -- halt
          when x"40" => next_state <= S_HALT;
          -- other instructions
          when others => next_state <= S_IDLE;
        end case;

      when S_PTR_INC =>
        pc_inc <= '1';
        next_state <= S_FETCH;

      when S_PTR_DEC =>
        pc_inc <= '1';
        next_state <= S_FETCH;

      when S_VAL_INC_1 =>
        DATA_RDWR <= '0';
        mx1_select <= '0';
        next_state <= S_VAL_INC_2;
      when S_VAL_INC_2 =>
        mx2_select <= "11";
        next_state <= S_VAL_INC_3;
      when S_VAL_INC_3 =>
        DATA_RDWR <= '1';
        mx1_select <= '1';
        pc_inc <= '1'; 
        next_state <= S_FETCH;
      --when S_VAL_DEC =>

      when S_VAL_DEC_1 =>
        DATA_RDWR <= '0';
        mx1_select <= '0';
        next_state <= S_VAL_DEC_2;
      when S_VAL_DEC_2 =>
        mx2_select <= "10";
        next_state <= S_VAL_DEC_3;
      when S_VAL_DEC_3 =>
        DATA_RDWR <= '1';
        mx1_select <= '1';
        pc_inc <= '1'; 
        next_state <= S_FETCH;
      
      when S_WHILE_LEFT =>
        pc_inc <= '0';
        if DATA_RDATA = "00000000" then
          cnt_inc <= '1';
          next_state <= S_WHILE_LEFT_SKIP;
        else
          next_state <= S_FETCH;
        end if;

      when S_WHILE_LEFT_SKIP =>
        DATA_RDWR <= '1';
        mx1_select <= '1';
        if cnt = x"00" then
          pc_inc <= '0';
          next_state <= S_FETCH;
        else
          pc_inc <= '1';
          next_state <= S_WHILE_LEFT_SKIP;
          if DATA_RDATA = x"5B" then
            cnt_inc <= '1';
          elsif DATA_RDATA = x"5D" then
            next_state <= S_WHILE_LEFT_RBRACKET;
            pc_inc <= '0';
            cnt_dec <= '1';      
          end if;
        end if;
      
      when S_WHILE_LEFT_RBRACKET =>
        if cnt = x"00" then
          next_state <= S_FETCH;
        else 
          pc_inc <= '1';
          next_state <= S_WHILE_LEFT_SKIP;
        end if;

      when S_WHILE_RIGHT =>  
        if DATA_RDATA = "00000000" then
          DATA_RDWR <= '1';
          mx1_select <= '1';
          pc_inc <= '1';
          next_state <= S_FETCH;
        else
          cnt_inc <= '1';
          pc_dec <= '1';
          next_state <= S_WHILE_RIGHT_SKIP_1;
        end if;
      when S_WHILE_RIGHT_SKIP_1 =>
        DATA_RDWR <= '1';
        mx1_select <= '1';
        next_state <= S_WHILE_RIGHT_SKIP;
      when S_WHILE_RIGHT_SKIP =>
        DATA_RDWR <= '1';
        mx1_select <= '1';
        if cnt = x"00" then
          pc_dec <= '0';
          next_state <= S_FETCH;
        else
          pc_dec <= '1';
          next_state <= S_WHILE_RIGHT_SKIP;
          if DATA_RDATA = x"5B" then
            next_state <= S_WHILE_RIGHT_LBRACKET;
            pc_dec <= '0';
            cnt_dec <= '1';
          elsif DATA_RDATA = x"5D" then
            cnt_inc <= '1';        
          end if;
        end if;

      when S_WHILE_RIGHT_LBRACKET =>
        if cnt = x"00" then
          pc_inc <= '1';
          next_state <= S_FETCH;
        else 
          pc_dec <= '1';
          next_state <= S_WHILE_RIGHT_SKIP;
        end if;

      when S_PRINT_1 =>
        OUT_DATA <= DATA_RDATA;
        if OUT_BUSY = '1' then
          next_state <= S_PRINT_1;
        else
          OUT_WE <= '1';
          next_state <= S_PRINT_2;
        end if;

      when S_PRINT_2 =>
        DATA_RDWR <= '1';
        mx1_select <= '0';
        OUT_WE <= '0';
        pc_inc <= '1';
        next_state <= S_FETCH;

      when S_INPUT_1 =>
        if IN_VLD = '0' then
          DATA_RDWR <= '0';
          mx1_select <= '0';
          mx2_select <= "00";
          next_state <= S_INPUT_1;
        else
          DATA_RDWR <= '0';
          mx1_select <= '0';
          mx2_select <= "00";
          next_state <= S_INPUT_2;
        end if;

      when S_INPUT_2 =>
        IN_REQ <= '0';
        mx1_select <= '1';
        pc_inc <= '1';
        DATA_RDWR <= '1';
        next_state <= S_FETCH;
    
      when S_TMP_PUT =>
        tmp_load <= '1';
        pc_inc <= '1';
        next_state <= S_FETCH;

      when S_TMP_GET =>
        pc_inc <= '1';
        next_state <= S_FETCH;

      when S_HALT => 
        DONE <= '1';
        next_state <= S_START;
      when S_IDLE =>
        mx1_select <= '0';
        pc_inc <= '1';
        next_state <= S_FETCH;
    end case;
  end process;
end behavioral;

