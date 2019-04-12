-- fsm.vhd: Finite State Machine
-- Author(s): Jan Pawlus
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (TEST1, TEST2, TEST3, TEST4, TEST5, TEST6, TEST7_1, TEST7_2, TEST8_1, TEST8_2, TEST9_1, TEST9_2, TEST10_2, WRONG, RIGHT, PRINT_MESSAGE_WRONG, PRINT_MESSAGE_RIGHT, FINISH);
   signal present_state, next_state : t_state;
   signal cntt : integer := 0;
  

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------

next_state_logic : process(present_state, KEY, CNT_OF)
variable cnt : integer := 0;
   variable kod : integer := 0;
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1 =>
      if (KEY(8) = '1') then
         next_state <= TEST2;       
      elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG; 
      else
         next_state <= TEST1;          
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -

   when TEST2=>
      if (KEY(6) = '1') then
         next_state <= TEST3;       
     elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG; 
      else
         next_state <= TEST2;            
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

   when TEST3=>
      if (KEY(3) = '1') then
         next_state <= TEST4;       
      elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG; 
      else
         next_state <= TEST3;            
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

   when TEST4=>
      if (KEY(7) = '1') then
         next_state <= TEST5;       
      elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG; 
      else
         next_state <= TEST4;           
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

   when TEST5=>
      if (KEY(8) = '1') then
         next_state <= TEST6;       
      elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG; 
      else
         next_state <= TEST5;          
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

   when TEST6=>
      if (KEY(0) = '1') then
         next_state <= TEST7_1;    
      elsif (KEY(1) = '1') then
         next_state <= TEST7_2;   
      elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG; 
      else
         next_state <= TEST6;            
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

    when TEST7_1=>
      if (KEY(2) = '1') then
         next_state <= TEST8_1;       
      elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG; 
      else
         next_state <= TEST7_1;            
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

   when TEST7_2=>
      if (KEY(3) = '1') then
         next_state <= TEST8_2;      
      elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG; 
      else
         next_state <= TEST7_2;            
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

    when TEST8_1=>
      if (KEY(2) = '1') then
         next_state <= TEST9_1;       
      elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG; 
      else
         next_state <= TEST8_1;           
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

    when TEST8_2=>
      if (KEY(9) = '1') then
         next_state <= TEST9_2;       
      elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG;  
      else
         next_state <= TEST8_2;           
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

    when TEST9_1=>
      if (KEY(4) = '1') then
         next_state <= RIGHT;       
      elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG; 
      else
         next_state <= TEST9_1;           
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

    when TEST9_2=>
      if (KEY(3) = '1') then
         next_state <= TEST10_2;       
      elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG; 
      else
         next_state <= TEST9_2;           
      end if;

   -- - - - - - - - - - - - - - - - - - - - - - -

    when TEST10_2=>
      if (KEY(0) = '1') then
         next_state <= RIGHT;       
      elsif (KEY(15) = '1') then 
         next_state <= PRINT_MESSAGE_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG; 
      else
         next_state <= TEST10_2;           
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when WRONG =>
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG;
      else
         next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when RIGHT =>
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_RIGHT;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= WRONG;         
      else
         next_state <= RIGHT;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_RIGHT =>      
      next_state <= PRINT_MESSAGE_RIGHT;
      if (CNT_OF = '1') then         
         next_state <= FINISH;
    
      end if;   
   -- - - - - - - - - - - - - - - - - - - - - - -

   when PRINT_MESSAGE_WRONG =>      
      next_state <= PRINT_MESSAGE_WRONG;
      if (CNT_OF = '1') then         
         next_state <= FINISH;
    
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      
      if (KEY(15) = '1') then
         next_state <= TEST1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';   
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';  

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST2 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST3 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST4 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST5 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST6 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST7_1 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST7_2 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST8_1 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST8_2 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST9_1 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST9_2 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST10_2 =>   
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;                
       -- - - - - - - - - - - - - - - - - - - - - - -
   when RIGHT =>        
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR  <= '1';         
      end if;      
       -- - - - - - - - - - - - - - - - - - - - - - -
   when WRONG =>       
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';         
      end if;                         

   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_RIGHT =>        
      FSM_MX_MEM <= '1';
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';      
      FSM_LCD_WR     <= '1';

      -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_WRONG =>   
      FSM_MX_MEM <= '0';        
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';      
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';

      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
   end case;
end process output_logic;

end architecture behavioral;

