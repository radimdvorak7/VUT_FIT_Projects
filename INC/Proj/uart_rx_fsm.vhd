-- uart_rx_fsm.vhd: UART controller - finite state machine controlling RX side
-- Author(s): Radim Dvořák (xdvorar00)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity UART_RX_FSM is
    port(
       CLK : in std_logic;
       RST : in std_logic;
       SDIN : in std_logic;
       START : in std_logic;
       LASTBIT : in std_logic;
       STOP : in std_logic;
       START_EN : out std_logic;
       STOP_EN : out std_logic;
       CNT_EN : out std_logic;
       MEM_EN : out std_logic);
end entity;

architecture behavioral of UART_RX_FSM is
     type t_state is (IDLE, START_BIT, DATA, STOP_BIT);
     signal current_state : t_state;
     signal next_state : t_state := IDLE;
begin
    --State register
    state_register: process(CLK,RST)
    begin
        if rising_edge(CLK) then
            if(RST='1') then
                current_state <= IDLE;
            else
                current_state <= next_state;
            end if;
        end if;
    end process;

    -- Next state logic
    next_state_logic: process(current_state, SDIN, START, LASTBIT, STOP, RST)
    begin
        next_state <= current_state;
        if RST = '1' then
            next_state <= IDLE;
        else
            case current_state is
                when IDLE =>
                    if SDIN = '0' then
                        next_state <= START_BIT;
                    end if;
                when START_BIT =>
                    if START = '1' then
                        next_state <= DATA;
                    end if;
                when DATA =>
                    if LASTBIT = '1' then
                        next_state <= STOP_BIT;
                    end if;
                when STOP_BIT =>
                    if STOP = '1' then
                        next_state <= IDLE;
                    end if;
            end case;
        end if;
    end process;

     -- Output combinatorial logic
    output_logic: process(current_state)
    begin
        START_EN <= '0';
        STOP_EN <= '0';
        CNT_EN <= '0';
        MEM_EN <= '0';

        case current_state is
            when IDLE =>
                START_EN <= '0';
                STOP_EN <= '0';
                CNT_EN <= '0';
                MEM_EN <= '0';
            when START_BIT =>
                START_EN <= '1';
                STOP_EN <= '0';
                CNT_EN <= '0';
                MEM_EN <= '0';
            when DATA =>
                START_EN <= '0';
                STOP_EN <= '0';
                CNT_EN <= '1';
                MEM_EN <= '1';
            when STOP_BIT =>
                START_EN <= '0';
                STOP_EN <= '1';
                CNT_EN <= '0';
                MEM_EN <= '1';
        end case;
    end process;

end architecture;
