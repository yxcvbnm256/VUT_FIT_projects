library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;


entity ledc8x8 is
	port (  SMCLK, RESET: in std_logic;
            ROW, LED: out std_logic_vector(0 to 7));
 end entity ledc8x8;

architecture behavioral of ledc8x8 is	
    signal en: std_logic;    
    subtype rotate_sub is integer range 0 to 7;
	signal cnt : std_logic_vector(0 to 7);
	signal rotate : rotate_sub := 0;
begin
	process(SMCLK, RESET, cnt)
	begin
		if (RESET = '1') then
			cnt <= (others => '0');			
		elsif rising_edge(SMCLK) then
			cnt <= cnt + 1;
	    	if cnt = cnt'high then
	        	cnt <= (others => '0');
	        	en <= '1';
	      	else	        
				en <= '0';
	      	end if;
	    end if;
    end process;

    process(en, SMCLK, RESET)
    begin
    	if (RESET = '1') then
    		rotate <= 0;
    	elsif (rising_edge(SMCLK) and (en = '1')) then
    		rotate <= (rotate+1) mod 8; 
    	end if;
    end process;

    process(rotate)
    begin    	
	    case rotate is
	    	WHEN 0 => LED <= "00001111"; ROW <= "10000000";
	    	WHEN 1 => LED <= "11100001"; ROW <= "01000000";    		
	    	WHEN 2 => LED <= "11100110"; ROW <= "00100000";
	    	WHEN 3 => LED <= "11100110"; ROW <= "00010000";
	    	WHEN 4 => LED <= "11100001"; ROW <= "00001000";
	    	WHEN 5 => LED <= "01100111"; ROW <= "00000100";
	    	WHEN 6 => LED <= "10010111"; ROW <= "00000010";
	    	WHEN 7 => LED <= "11110111"; ROW <= "00000001";
	    end case;    	
    end process;
end behavioral;