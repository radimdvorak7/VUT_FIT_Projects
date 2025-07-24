-- uart_rx.vhd: UART controller - receiving (RX) side
-- Author(s): Radim Dvořák (xdvorar00)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;



-- Entity declaration (DO NOT ALTER THIS PART!)
entity UART_RX is
    port(
        CLK      : in std_logic;
        RST      : in std_logic;
        DIN      : in std_logic;
        DOUT     : out std_logic_vector(7 downto 0);
        DOUT_VLD : out std_logic
    );
end entity;



-- Architecture implementation (INSERT YOUR IMPLEMENTATION HERE)
architecture behavioral of UART_RX is
    signal START : std_logic;
    signal STOP : std_logic;
    signal SDIN : std_logic;
    signal LASTBIT : std_logic;
    signal START_COUNT : std_logic_vector(2 downto 0);
    signal STOP_COUNT : std_logic_vector(3 downto 0);
    signal CLK_COUNT : std_logic_vector(3 downto 0);
    signal BIT_ADRESS : std_logic_vector(2 downto 0);
    signal DOUT_DEC : std_logic_vector(7 downto 0);
    signal SAMPLE : std_logic;
    signal START_BIT : std_logic;
    signal START_EN : std_logic;
    signal STOP_EN : std_logic;
    signal CNT_EN : std_logic;
    signal MEM_EN: std_logic;
begin
    -- Instance of RX FSM
    fsm: entity work.UART_RX_FSM
    port map (
        CLK => CLK,
        RST => RST,
        SDIN => DIN,
        START => START,
        LASTBIT => LASTBIT,
        STOP => STOP,
        START_EN => START_EN,
        STOP_EN => STOP_EN,
        CNT_EN => CNT_EN,
        MEM_EN => MEM_EN
    );

    -- Start counter
    start_cnt: process(CLK, START_EN)
    begin
        if START_EN = '0' then
            START_COUNT <= (others => '0');
        elsif rising_edge(CLK) then
            if START_EN = '1' then
                START_COUNT <= START_COUNT + 1;
            end if;
        end if;
    end process;

    -- Start comparator
    start_cmp: process(CLK, START_EN, START_COUNT)
    begin
        if START_EN = '0' then
            START <= '0';
        elsif rising_edge(CLK) then
            if START_EN = '1' and  START_COUNT = "111" then
                START <= '1';
            else
                START <= '0';
            end if;
        end if;
    end process;

    -- Stop counter
    stop_cnt:process(CLK, STOP_EN)
    begin
        if STOP_EN = '0' then
            STOP_COUNT <= (others => '0');
        elsif rising_edge(CLK) then
            if STOP_EN = '1' then
                STOP_COUNT <= STOP_COUNT + 1;
            end if;
        end if;
    end process;

    -- Stop midbit comparator
    stop_cmp:process(CLK, STOP_EN, STOP_COUNT)
    begin
        if STOP_EN = '0' then
            STOP <= '0';
        elsif rising_edge(CLK) then
            if STOP_EN = '1' and STOP_COUNT = "1110" then
                STOP <= '1';
            else
                STOP <= '0';
            end if;
        end if;
    end process;

    -- Counter 16x clock
    clk_cnt:process(CLK, CNT_EN)
    begin
        if CNT_EN = '0' then
            CLK_COUNT <= (others => '0');
        elsif rising_edge(CLK) then
            if CNT_EN = '1' then
                CLK_COUNT <= CLK_COUNT + 1;
            end if;
        end if;
    end process;

    --Comparator for taking sample in midbit
    bit_cmp:process(CLK, CNT_EN, CLK_COUNT)
    begin
        if CNT_EN = '0' then
            SAMPLE <= '0';
        elsif rising_edge(CLK) then
            if CNT_EN = '1' and CLK_COUNT = "1101" then
                SAMPLE <= '1';
            else
                SAMPLE <= '0';
            end if;
        end if;
    end process;

    --Counter for bit address
    bit_adress_cnt:process(CLK, SAMPLE, CNT_EN)
    begin
        if CNT_EN = '0' then
            BIT_ADRESS <= (others => '0');
        elsif rising_edge(CLK) then
            if SAMPLE = '1' then
                BIT_ADRESS <= BIT_ADRESS + 1;
            end if;
        end if;
    end process;

    --Comparator if 8 bit were read
    bit_adress_cmp:process(SAMPLE, CNT_EN, BIT_ADRESS)
    begin
        if CNT_EN = '0' then
            LASTBIT <= '0';
        elsif rising_edge(SAMPLE) then
            if CNT_EN = '1' and BIT_ADRESS = "111" then
                LASTBIT <= '1';
            else
                LASTBIT <= '0';
            end if;
        end if;
    end process;
    
    --Output decoder
    out_dec: process(BIT_ADRESS)
    begin
        DOUT_DEC <= "00000000";
        if MEM_EN = '0' then
            DOUT_DEC <= "00000000";
        end if;
        case BIT_ADRESS is
            when "000" => DOUT_DEC <= "00000001";
            when "001" => DOUT_DEC <= "00000010";
            when "010" => DOUT_DEC <= "00000100";
            when "011" => DOUT_DEC <= "00001000";
            when "100" => DOUT_DEC <= "00010000";
            when "101" => DOUT_DEC <= "00100000";
            when "110" => DOUT_DEC <= "01000000";
            when "111" => DOUT_DEC <= "10000000";
            when others => DOUT_DEC <= "00000000";
        end case;
    end process;
    
    --Output registers
    out_reg0: process(SAMPLE,MEM_EN)
    begin
        if MEM_EN = '0' then
            DOUT(0) <= '0';
        elsif rising_edge(SAMPLE) then
            if(DOUT_DEC(0) = '1') then
                DOUT(0) <= DIN;
            end if;
        end if;
    end process;  

    out_reg1: process(SAMPLE,MEM_EN)
    begin
        if MEM_EN = '0' then
            DOUT(1) <= '0';
        elsif rising_edge(SAMPLE) then
            if(DOUT_DEC(1) = '1') then
                DOUT(1) <= DIN;
            end if;
        end if;
    end process;

    out_reg2: process(SAMPLE,MEM_EN)
    begin
        if MEM_EN = '0' then
            DOUT(2) <= '0';
        elsif rising_edge(SAMPLE) then
            if(DOUT_DEC(2) = '1') then
                DOUT(2) <= DIN;
            end if;
        end if;
    end process;    

    out_reg3: process(SAMPLE,MEM_EN)
    begin
        if MEM_EN = '0' then
            DOUT(3) <= '0';
        elsif rising_edge(SAMPLE) then
            if(DOUT_DEC(3) = '1') then
                DOUT(3) <= DIN;
            end if;
        end if;
    end process;   

    out_reg4: process(SAMPLE,MEM_EN)
    begin
        if MEM_EN = '0' then
            DOUT(4) <= '0';
        elsif rising_edge(SAMPLE) then
            if(DOUT_DEC(4) = '1') then
                DOUT(4) <= DIN;
            end if;
        end if;
    end process;  

    out_reg5: process(SAMPLE,MEM_EN)
    begin
        if MEM_EN = '0' then
            DOUT(5) <= '0';
        elsif rising_edge(SAMPLE) then
            if(DOUT_DEC(5) = '1') then
                DOUT(5) <= DIN;
            end if;
        end if;
    end process;

    out_reg6: process(SAMPLE,MEM_EN)
    begin
        if MEM_EN = '0' then
            DOUT(6) <= '0';
        elsif rising_edge(SAMPLE) then
            if(DOUT_DEC(6) = '1') then
                DOUT(6) <= DIN;
            end if;
        end if;
    end process;

    out_reg7: process(SAMPLE,MEM_EN)
    begin
        if MEM_EN = '0' then
            DOUT(7) <= '0';
        elsif rising_edge(SAMPLE) then
            if(DOUT_DEC(7) = '1') then
                DOUT(7) <= DIN;
            end if;
        end if;
    end process;  

    DOUT_VLD <= STOP;

end architecture;
