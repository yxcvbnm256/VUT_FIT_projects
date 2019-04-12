library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned.all;
use IEEE.numeric_std.all;
use IEEE.std_logic_arith.all;


entity sid6581 is
	port (
		clk40			: in std_logic;					
		clk1mhz   		: in std_logic;              
		reset			: in std_logic;								
		cs				: in std_logic;								
		we				: in std_logic;	
		addr			: in unsigned(4 downto 0);				--  "adresa" rikajici, jaka data prichazi
		di				: in std_logic_vector(7 downto 0);		--	data prichazejici z FLASH pameti
		do				: out std_logic_vector(7 downto 0);		--	vystupni data ze SID cipu		
		audio_data		: out std_logic_vector(17 downto 0)
	);
end sid6581;

architecture Behavioral of sid6581 is
	component voice_gen is
		port (
			clk_1MHz	: in std_logic;							 
			reset		: in std_logic;							 
			fq_low		: in std_logic_vector(7 downto 0);	-- registr frekvence
			fq_high		: in std_logic_vector(7 downto 0);	-- registr frekvence			
			control		: in std_logic_vector(7 downto 0);	-- udava typ tonu - obdelnik, puls, trojuhelnik, pila, sum
			env_att_dec	: in std_logic_vector(7 downto 0);	-- informace pro generator obalky
			env_sus_rel	: in std_logic_vector(7 downto 0);	-- informace pro generator obalky			
			osc			: out std_logic_vector(7 downto 0);	--
			env			: out std_logic_vector(7 downto 0);	--
			voice		: out std_logic_vector(11 downto 0)	-- skutecny audio signal
			);
	end component;


	-- signaly nesouci informace o tonech
	signal clk_1MHz			: std_logic;
	signal devide_0			: std_logic_vector(31 downto 0);
	signal voice1			: std_logic_vector(11 downto 0);
	signal voice2			: std_logic_vector(11 downto 0);
	signal voice3			: std_logic_vector(11 downto 0);
	signal voice_mixed		: std_logic_vector(13 downto 0);
	signal voice_volume		: std_logic_vector(17 downto 0);

	signal voice1_fq_low	: std_logic_vector(7 downto 0) := (others => '0');
	signal voice1_fq_high	: std_logic_vector(7 downto 0) := (others => '0');	
	signal voice1_control	: std_logic_vector(7 downto 0) := (others => '0');
	signal voice1_env_att_dec	: std_logic_vector(7 downto 0) := (others => '0');
	signal voice1_env_sus_rel	: std_logic_vector(7 downto 0) := (others => '0');

	signal voice2_fq_low	: std_logic_vector(7 downto 0) := (others => '0');
	signal voice2_fq_high	: std_logic_vector(7 downto 0) := (others => '0');	
	signal voice2_control	: std_logic_vector(7 downto 0) := (others => '0');
	signal voice2_env_att_dec	: std_logic_vector(7 downto 0) := (others => '0');
	signal voice2_env_sus_rel	: std_logic_vector(7 downto 0) := (others => '0');

	signal voice3_fq_low	: std_logic_vector(7 downto 0) := (others => '0');
	signal voice3_fq_high	: std_logic_vector(7 downto 0) := (others => '0');	
	signal voice3_control	: std_logic_vector(7 downto 0) := (others => '0');
	signal voice3_env_att_dec	: std_logic_vector(7 downto 0) := (others => '0');
	signal voice3_env_sus_rel	: std_logic_vector(7 downto 0) := (others => '0');

	signal filter_vol		: std_logic_vector(7 downto 0) := (others => '0');

	begin
		audio_data <= voice_volume;
		voice_gen1: voice_gen
			port map(
				voice		=> voice1,
				clk_1MHz	=> clk_1MHz,
				reset		=> reset,
				fq_low		=> voice1_fq_low,
				fq_high		=> voice1_fq_high,				
				control		=> voice1_control,
				env_att_dec	=> voice1_env_att_dec,
				env_sus_rel	=> voice1_env_sus_rel					
				);

		voice_gen2: voice_gen
			port map(
				voice		=> voice2,
				clk_1MHz	=> clk_1MHz,
				reset		=> reset,
				fq_low		=> voice2_fq_low,
				fq_high		=> voice2_fq_high,				
				control		=> voice2_control,
				env_att_dec	=> voice2_env_att_dec,
				env_sus_rel	=> voice2_env_sus_rel				
				);

		voice_gen3: voice_gen
			port map(
				voice		=> voice3,
				clk_1MHz	=> clk_1MHz,
				reset		=> reset,
				fq_low		=> voice3_fq_low,
				fq_high		=> voice3_fq_high,				
				control		=> voice3_control,
				env_att_dec	=> voice3_env_att_dec,
				env_sus_rel	=> voice3_env_sus_rel				
				);

		clk_1MHz <= clk1mhz;
		voice_mixed	<= (("00" & voice1) + ("00" & voice2)) + ("00" & voice3); -- mixovani zvuku - secteni
		voice_volume <= voice_mixed * filter_vol(3 downto 0); -- zvyseni hlasitosti	-> vystup z oscilatoru je sam o sobe hodne potichu

		register_decoder:process(clk40)
		begin
			if clk40='1' and clk40'event then
				if reset = '1' then
					-- resetovani
					voice1_fq_low		<= (others => '0');
					voice1_fq_high		<= (others => '0');					
					voice1_control		<= (others => '0');
					voice1_env_att_dec	<= (others => '0');
					voice1_env_sus_rel	<= (others => '0');
					
					voice2_fq_low		<= (others => '0');
					voice2_fq_high		<= (others => '0');					
					voice2_control		<= (others => '0');
					voice2_env_att_dec	<= (others => '0');
					voice2_env_sus_rel	<= (others => '0');
					
					voice3_fq_low		<= (others => '0');
					voice3_fq_high		<= (others => '0');					
					voice3_control		<= (others => '0');
					voice3_env_att_dec	<= (others => '0');
					voice3_env_sus_rel	<= (others => '0');	
					filter_vol			<= (others => '0');				
				else
					voice1_fq_low		<= voice1_fq_low;
					voice1_fq_high		<= voice1_fq_high;					
					voice1_control		<= voice1_control;
					voice1_env_att_dec	<= voice1_env_att_dec;
					voice1_env_sus_rel	<= voice1_env_sus_rel;
					voice2_fq_low		<= voice2_fq_low;
					voice2_fq_high		<= voice2_fq_high;					
					voice2_control		<= voice2_control;
					voice2_env_att_dec	<= voice2_env_att_dec;
					voice2_env_sus_rel	<= voice2_env_sus_rel;
					voice3_fq_low		<= voice3_fq_low;
					voice3_fq_high		<= voice3_fq_high;					
					voice3_control		<= voice3_control;
					voice3_env_att_dec	<= voice3_env_att_dec;
					voice3_env_sus_rel	<= voice3_env_sus_rel;
					filter_vol			<= filter_vol;
					
					if (cs='1') then
						if (we='1') then
							-- prirazeni potrebnych informaci zasilanych z FLASH pameti signalum
							case to_integer(addr) is
								
								when 00 =>	voice1_fq_low		<= di;
								when 01 =>	voice1_fq_high		<= di;								
								when 04 =>	voice1_control		<= di;
								when 05 =>	voice1_env_att_dec	<= di;
								when 06 =>	voice1_env_sus_rel	<= di;
								
								when 07 =>	voice2_fq_low		<= di;
								when 08 =>	voice2_fq_high		<= di;								
								when 11 =>	voice2_control		<= di;
								when 12 =>	voice2_env_att_dec	<= di;
								when 13 =>	voice2_env_sus_rel	<= di;
								
								when 14 =>	voice3_fq_low		<= di;
								when 15 =>	voice3_fq_high		<= di;								
								when 18 =>	voice3_control		<= di;
								when 19 =>	voice3_env_att_dec	<= di;
								when 20 =>	voice3_env_sus_rel	<= di;

								when 24 =>	filter_vol			<= di;								
								when others	=>	null;
							end case;				
						end if;
					end if;
				end if;
			end if;
		end process;


end Behavioral;
	
