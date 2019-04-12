-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2014 Brno University of Technology,
--                    Faculty of Information Technology
-- Author: Jan Pawlus (xpawlu00)
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
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
    signal pc_reg : std_logic_vector(11 downto 0);
    signal pc_dec : std_logic;
    signal pc_inc : std_logic;
    signal ptr_reg : std_logic_vector(11 downto 0);
    signal ptr_dec : std_logic;
    signal ptr_inc : std_logic;

    signal cnt_reg: std_logic_vector(11 downto 0) := "000000000000";
    signal cnt_inc: std_logic;
    signal cnt_dec: std_logic;
    signal cnt_forceone: std_logic;
    signal mx1 :std_logic;

    type fsm_state is (idle, other, fetch, fetch1, decode, ptr_increment, ptr_decrement, data_increment, data_increment1, data_decrement, data_decrement1, 
    left_bracket, right_bracket, print, print1, read_data, read_data1, halt, left_bracket1, left_bracket2, left_bracket3, left_bracket4, 
    right_bracket1, right_bracket2, right_bracket3, right_bracket4, right_bracket5, right_bracket6, heybabywakeupfromyourasleep);
    signal pstate: fsm_state;
    signal nstate: fsm_state;
begin
    --ASYNCHRONNÍ MULTIPLEXOR
    DATA_ADDR <= '0' & pc_reg when mx1 = '0' else '1' & ptr_reg;
    
    --COUNTER PRO WHILE
    cnt_counter: process (RESET, CLK)
    begin
        if (RESET = '1') then
            cnt_reg <= (others => '0');
        elsif (rising_edge(CLK)) then
            if (cnt_forceone = '1') then
                cnt_reg <= "000000000001";
            elsif (cnt_inc='1') then
                cnt_reg <= cnt_reg + 1;
            elsif (cnt_dec='1') then
                cnt_reg <= cnt_reg - 1;
            end if;             
        end if;
    end process;

    --COUNTER PRO PC
    pc_counter: process (RESET, CLK)
    begin
        if (RESET='1') then
            pc_reg <= (others=>'0');
        elsif (rising_edge(CLK) and EN = '1') then
            if (pc_dec='1') then
                pc_reg <= pc_reg - 1;
            elsif (pc_inc='1') then
                pc_reg <= pc_reg + 1;
            end if;
        end if;
    end process;

    ptr_counter: process (RESET, CLK)
    begin
        if (RESET = '1') then
            ptr_reg <= "000000000000";
        elsif (rising_edge(CLK) and EN = '1') then
            if (ptr_inc = '1') then
                ptr_reg <= ptr_reg + 1;          
            elsif (ptr_dec = '1') then
                ptr_reg <= ptr_reg - 1;        
        end if;
      end if;     
    end process;
   
    fsm_present: process (RESET, CLK, EN)
    begin
        if (RESET = '1') then
            pstate <= idle;
        elsif (rising_edge(CLK) and EN='1') then
            pstate <= nstate;
        end if;         
    end process;


    nsl: process (pstate, OUT_BUSY, cnt_reg, ptr_reg, pc_reg, DATA_RDATA, IN_VLD, IN_DATA, mx1, EN)
    begin
        mx1 <= '0';
        nstate <= idle;
        cnt_inc <= '0';
        cnt_dec <= '0'; 
        pc_inc <= '0';
        pc_dec <= '0';
        ptr_inc <= '0';
        ptr_dec <= '0';        
        cnt_forceone <= '0';
        DATA_EN <= '0';
        DATA_RDWR <= '0';
        OUT_WE <= '0';
        OUT_DATA <= "00000000";
        DATA_WDATA <= "00000000";
        DATA_RDWR <= '0';
        IN_REQ <= '0';         
        if (EN = '1') then
            case pstate is

                when idle =>
                    nstate <= fetch;

                when fetch =>
                    DATA_RDWR <= '0';  
                    mx1 <= '0';  
                    DATA_EN <= '1';       
                    nstate <= decode;

               when decode =>            
                  if (mx1 = '0') then
                      case DATA_RDATA is
                          when X"3E" => nstate <= ptr_increment;
                          when X"3C" => nstate <= ptr_decrement;
                          when X"2B" => nstate <= data_increment;
                          when X"2D" => nstate <= data_decrement;
                          when X"5B" => nstate <= left_bracket;
                          when X"5D" => nstate <= right_bracket;
                          when X"2E" => nstate <= print;
                          when X"2C" => nstate <= read_data;
                          when X"00" => nstate <= halt;
                          when others => nstate <= other;
                      end case;
                  end if;

            when ptr_increment =>
                ptr_inc <= '1';
                pc_inc <= '1';
                mx1 <= '1';
                nstate <= fetch;          

            when ptr_decrement =>
                ptr_dec <= '1';
                pc_inc <= '1';
                nstate <= fetch;
                mx1 <= '1'; 

            when data_increment =>
                mx1 <= '1'; 
                DATA_EN <= '1';
                DATA_RDWR <= '0';
                nstate <= data_increment1;

            when data_increment1 =>
                mx1 <= '1'; 
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                DATA_WDATA <= DATA_RDATA +1;
                pc_inc <= '1';
                nstate <= fetch;

            when data_decrement =>
                mx1 <= '1'; 
                DATA_EN <= '1';
                DATA_RDWR <= '0';          
                nstate <= data_decrement1;

            when data_decrement1 =>
                mx1 <= '1'; 
                DATA_EN <= '1';
                DATA_RDWR <= '1';
                DATA_WDATA <= DATA_RDATA -1;
                pc_inc <= '1';
                nstate <= fetch;

            when print =>
                mx1 <= '1';
                DATA_EN <= '1';
                DATA_RDWR <= '0';
                nstate <= print1; 

            when print1 =>
                if (OUT_BUSY = '0') then
                    OUT_WE <= '1';
                    pc_inc <= '1';
                    nstate <= fetch; 
                    OUT_DATA <= DATA_RDATA;
                else
                    nstate <= print1;
                end if;

            when read_data =>
                IN_REQ <= '1';
                mx1 <= '1';
                nstate <= read_data1;

            when read_data1 =>
                if (IN_VLD = '1') then
                    mx1 <= '1'; 
                    IN_REQ <= '1';
                    DATA_EN <= '1';
                    DATA_RDWR <= '1';
                    DATA_WDATA <= IN_DATA;
                    IN_REQ <= '0';
                    pc_inc <= '1';
                    nstate <= fetch;
                else
                    nstate <= read_data1;    
                end if;

            when left_bracket =>          
                mx1 <= '1'; 
                DATA_EN <= '1';
                DATA_RDWR <= '0';
                nstate <= left_bracket1;
                pc_inc <= '1';

            when left_bracket1 =>
               
                if (DATA_RDATA = "00000000") then
                    cnt_forceone <= '1'; 
                    nstate <= left_bracket2;                
                else
                    nstate <= fetch;
                end if;

            when left_bracket2 =>  
                mx1 <= '0';      
                nstate <= left_bracket3;           
                DATA_EN <= '1';

            when left_bracket3 =>          
                    if (DATA_RDATA = X"5B") then
                        cnt_inc <= '1';
                    elsif (DATA_RDATA = X"5D") then
                        cnt_dec <= '1';
                    end if;
                    nstate <= left_bracket4;
                    pc_inc <= '1';         

            when left_bracket4 =>
                if (cnt_reg = "0000000000000") then
                   nstate <= fetch;
                else
                    nstate <= left_bracket2;
                end if;

            when right_bracket =>
                DATA_EN <= '1';
                mx1 <= '1'; 
                DATA_RDWR <= '0';
                nstate <= right_bracket1;

            when right_bracket1 =>
                if (DATA_RDATA = "00000000") then
                    pc_inc <= '1';             
                    nstate <= fetch;
                else
                    cnt_forceone <= '1';
                    pc_dec <= '1';              
                    nstate <= right_bracket2;
                end if;

            when right_bracket2 =>
                DATA_EN <= '1';
                mx1 <= '0';           
                nstate <= right_bracket3;

            when right_bracket3 =>  
                if (cnt_reg /= "00000000") then        
                    if (DATA_RDATA = X"5D") then
                        cnt_inc <= '1';                  
                    elsif (DATA_RDATA = X"5B") then
                        cnt_dec <= '1';                  
                    end if;  
                   nstate <= right_bracket4;
                else
                    nstate <= fetch;              
               end if;

            when right_bracket4 =>
                if (cnt_reg = "00000000") then                                        
                    pc_inc <= '1';                                          
                else                                        
                    pc_dec <= '1';                  
                end if;
                nstate <= right_bracket2;
                DATA_EN <= '1';      

            when other =>
                pc_inc <= '1';
                nstate <= fetch;

            when halt =>
                nstate <= halt;

            when others =>   
                     
          end case;
      end if;
    end process;
end behavioral;