library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned.all;
use IEEE.numeric_std.all;
use IEEE.std_logic_arith.all;

entity voice_gen is
	port (
		clk_1MHz		: in std_logic;
		reset			: in std_logic;
		fq_low			: in std_logic_vector(7 downto 0);
		fq_high			: in std_logic_vector(7 downto 0);
		control			: in std_logic_vector(7 downto 0);
		env_att_dec		: in std_logic_vector(7 downto 0);
		env_sus_rel		: in std_logic_vector(7 downto 0);
		
		osc				: out std_logic_vector(7 downto 0);
		env				: out std_logic_vector(7 downto 0);
		voice			: out std_logic_vector(11 downto 0)
	);
end voice_gen;

architecture Behavioral of voice_gen is	
	signal	accu_bit_prev			: std_logic;
	signal	accumulator				: std_logic_vector(23 downto 0);	
	signal	lfsr					: std_logic_vector(31 downto 0);
	signal 	frequency				: std_logic_vector(15 downto 0);

	-- generator obalky
	alias	gate					: std_logic is control(0); 				-- ovladaci bit GATE pro ADSR obalku 
	type	envelope_state_types is (idle, attack, attack_lp, decay, decay_lp, sustain, release, release_lp);
	signal 	cur_state, next_state 	: envelope_state_types; 
	signal 	divider_counter			: integer range 0 to 2**18 - 1;
	signal 	exp_table_value			: integer range 0 to 2**18 - 1;
	signal 	exp_table_active		: std_logic;
	signal 	divider_rst 			: std_logic;
	signal 	divider_value			: integer range 0 to 2**15 - 1;
	signal 	divider_attack			: integer range 0 to 2**15 - 1;
	signal 	divider_dec_rel			: integer range 0 to 2**15 - 1;
	signal	Dec_rel					: std_logic_vector(3 downto 0);
	signal	Dec_rel_sel				: std_logic;

	signal	env_counter				: std_logic_vector(7 downto 0);
	signal 	env_count_hold_A		: std_logic;
	signal	env_count_hold_B		: std_logic;
	signal	env_cnt_up				: std_logic;
	signal	env_cnt_clear			: std_logic;

	signal	osc_out					: std_logic_vector(11 downto 0);
	signal	signal_vol				: std_logic_vector(19 downto 0);
	
	begin
		osc						<= osc_out(11 downto 4);
		env						<= env_counter;
		frequency(15 downto 8)	<= fq_high(7 downto 0);
		frequency(7 downto 0)	<= fq_low(7 downto 0);	
		voice					<= signal_vol(19 downto 8);

		process(clk_1MHz) 
		begin
			if clk_1MHz'event and clk_1MHz = '1' then
		    	if reset = '1' then 
		    		accumulator <= (others => '0');
		 		else
		 	  		accumulator <= accumulator + ("0" & frequency(15 downto 0)); 
		    	end if;
			end if;
		end process;

		process(accumulator, control)
		begin
			osc_out <= "100000000000";
		    if (control(5) = '1') then                				-- pila
		        osc_out <= accumulator(23 downto 12);
		    elsif (control(6) = '1') then          					-- obdelnik
		        if (accumulator(23) = '1') then
		           osc_out <= (others => '1');
		        end if;
		    elsif (control(4) = '1') then           				-- trojuhelnik
		        if (accumulator(23) = '1') then
		           osc_out <= not accumulator(22 downto 11);
		        else
		           osc_out <= accumulator(22 downto 11);
		        end if;
		    elsif (control(7) = '1') then                           -- sum
		        osc_out <= lfsr(31 downto 20);
		    end if;		  
		end process;

		-- sum
		process(clk_1MHz) 
		begin
		  if (clk_1MHz'event) and (clk_1MHz='1') then  
		     if (accumulator(19) = '1') and (control(7)='1') then 
		        lfsr <= lfsr(30 downto 0) & (lfsr(31) xnor lfsr(21) xnor lfsr(1) xnor lfsr(0));
		  end if; end if;
		end process;




		-- GENERATOR OBALKY
		envelope_control_advance: process (reset, clk_1MHz)
		begin
			if (reset = '1') then
				cur_state <= idle;
			else
				if (clk_1MHz = '1' and clk_1MHz'event) then
					cur_state <= next_state;
				end if;
			end if;
		end process;


		-- ovladani generatoru obalky
		envelope_control: process (reset, cur_state, gate, divider_attack, divider_dec_rel, env_att_dec, env_sus_rel, env_counter)
		begin
			if (reset = '1') then
				next_state 				<= idle;
				env_cnt_clear			<='1';
				env_cnt_up				<='1';
				env_count_hold_B		<='1';
				divider_rst 			<='1';
				divider_value 			<= 0;
				exp_table_active 		<='0';
				Dec_rel_sel				<='0';
			else
				env_cnt_clear	 		<='0';
				env_cnt_up				<='1';
				env_count_hold_B		<='1';
				divider_rst 			<='0';
				divider_value 			<= 0;
				exp_table_active 		<='0';
				case cur_state is

					-- IDLE
					when idle =>
						env_cnt_clear 		<= '1';
						divider_rst 		<= '1';
						Dec_rel_sel			<='0';
						if gate = '1' then
							next_state 	<= attack;
						else
							next_state 	<= idle;
						end if;
					
					when attack =>
						env_cnt_clear		<= '1';
						divider_rst 		<= '1';
						divider_value 		<= divider_attack;
						next_state 			<= attack_lp;
						Dec_rel_sel			<='0';
					
					when attack_lp =>
						env_count_hold_B 	<= '0';
						env_cnt_up 			<= '1';
						divider_value 		<= divider_attack;
						Dec_rel_sel			<='0';
						if env_counter = "11111111" then
							next_state	<= decay;
						else
							if gate = '0' then
								next_state	<= release;
							else
								next_state	<= attack_lp;
							end if;
						end if;
				
					when decay =>
						divider_rst 		<= '1';
						exp_table_active 	<= '1';
						env_cnt_up	 		<= '0';
						divider_value 		<= divider_dec_rel;
						next_state 			<= decay_lp;
						Dec_rel_sel			<='0';
					
					when decay_lp =>
						exp_table_active 	<= '1';
						env_count_hold_B 	<= '0';
						env_cnt_up 			<= '0';
						divider_value 		<= divider_dec_rel;
						Dec_rel_sel			<='0';
						if (env_counter(7 downto 4) = env_sus_rel(7 downto 4)) then
							next_state 		<= sustain;
						else
							if gate = '0' then
								next_state		<= release;
							else
								next_state		<= decay_lp;
							end if;
						end if;
					
					when sustain =>
						divider_value 		<= 0;
						Dec_rel_sel			<='1';
						if gate = '0' then	
							next_state 			<= release;
						else
							if (env_counter(7 downto 4) = env_sus_rel(7 downto 4)) then
								next_state 		<= sustain;
							else
								next_state 		<= decay;						
							end if;
						end  if;
				
					when release =>
						divider_rst 		<= '1';
						exp_table_active 	<= '1';
						env_cnt_up	 		<= '0';
						divider_value 		<= divider_dec_rel;
						Dec_rel_sel			<='1';
						next_state 			<= release_lp;
							
					when release_lp =>
						exp_table_active 	<= '1';
						env_count_hold_B 	<= '0';
						env_cnt_up	 		<= '0';
						divider_value 		<= divider_dec_rel;
						Dec_rel_sel			<='1';
						if env_counter = "00000000" then
							next_state 	<= idle;
						else
							if gate = '1' then
								next_state 	<= idle;
							else
								next_state	<= release_lp;
							end if;
						end if;

					when others =>
							divider_value 		<= 0;
							Dec_rel_sel			<='0';
							next_state			<= idle;	
				end case;
			end if;
		end process;



		envelope_counter:process(clk_1MHz)
		begin
			if (clk_1MHz = '1' and clk_1MHz'event) then
				if ((reset = '1') or (env_cnt_clear = '1')) then
					env_counter <= "00000000";		
				else
					if ((env_count_hold_A = '1') or (env_count_hold_B = '1'))then
						env_counter <= env_counter;			
					else
						if (env_cnt_up = '1') then
								env_counter <= env_counter + 1;
						else
								env_counter <= env_counter - 1;
						end if;
					end if;
				end if;
			end if;
		end process;


		prog_fq_div:process(clk_1MHz)
		begin
			if (clk_1MHz = '1' and clk_1MHz'event) then
				if ((reset = '1') or (divider_rst = '1')) then
					env_count_hold_A 	<= '1';			
					divider_counter	<= 0;
				else
					if (divider_counter = 0) then
						env_count_hold_A 	<= '0';
						if (exp_table_active = '1') then
							divider_counter	<= exp_table_value;
						else
							divider_counter	<= divider_value;
						end if;
					else
						env_count_hold_A	<= '1';					
						divider_counter	<= divider_counter - 1;
					end if;
				end if;
			end if;
		end process;


		table:process(clk_1MHz)
		BEGIN
			if (clk_1MHz = '1' and clk_1MHz'event) then		
				if (reset = '1') then
					exp_table_value <= 0;
				else
					case CONV_INTEGER(env_counter) is
						when   0 to  51	=>	exp_table_value <= divider_value * 16;			--  51 to   0
						when  52 to 101 	=>	exp_table_value <= divider_value * 8;		--	101 to  52
						when 102 to 152 	=>	exp_table_value <= divider_value * 4;		--  152 to 102
						when 153 to 203 	=>	exp_table_value <= divider_value * 2;		--	203 to 153
						when 204 to 255 	=>	exp_table_value <= divider_value;			--  255 to 204
						when others			=>	exp_table_value <= divider_value;			--
					end case;
				end if;
			end if;
		end process;


		attack_table:process(clk_1MHz)
		begin
			if (clk_1MHz = '1' and clk_1MHz'event) then
				if (reset = '1') then
					divider_attack <= 0;
				else
					case env_att_dec(7 downto 4) is
						when "0000"	=>	divider_attack <= 8;		--attack rate: (2mS / 1uSec per clockcycle) / 254 steps
						when "0001" =>	divider_attack <= 31;		--attack rate: (8mS / 1uS per clockcycle) /254 steps
						when "0010" =>	divider_attack <= 63;		--attack rate: (16mS / 1uS per clockcycle) /254 steps
						when "0011" =>	divider_attack <= 94;		--attack rate: (24mS / 1uS per clockcycle) /254 steps
						when "0100" =>	divider_attack <= 150;		--attack rate: (38mS / 1uS per clockcycle) /254 steps
						when "0101" =>	divider_attack <= 220;		--attack rate: (56mS / 1uS per clockcycle) /254 steps
						when "0110" =>	divider_attack <= 268;		--attack rate: (68mS / 1uS per clockcycle) /254 steps
						when "0111" =>	divider_attack <= 315;		--attack rate: (80mS / 1uS per clockcycle) /254 steps
						when "1000" =>	divider_attack <= 394;		--attack rate: (100mS / 1uS per clockcycle) /254 steps
						when "1001" =>	divider_attack <= 984;		--attack rate: (250mS / 1uS per clockcycle) /254 steps
						when "1010" =>	divider_attack <= 1968;		--attack rate: (500mS / 1uS per clockcycle) /254 steps
						when "1011" =>	divider_attack <= 3150;		--attack rate: (800mS / 1uS per clockcycle) /254 steps
						when "1100" =>	divider_attack <= 3937;		--attack rate: (1000mS / 1uS per clockcycle) /254 steps
						when "1101" =>	divider_attack <= 11811;	--attack rate: (3000mS / 1uS per clockcycle) /254 steps
						when "1110" =>	divider_attack <= 19685;	--attack rate: (5000mS / 1uS per clockcycle) /254 steps
						when "1111" =>	divider_attack <= 31496; 	--attack rate: (8000mS / 1uS per clockcycle) / 254steps
						when others =>	divider_attack <= 0;
					end case;
				end if;
			end if;
		end process;


		decay_release_input_select:process(Dec_rel_sel, env_att_dec, env_sus_rel)
		begin
			if (Dec_rel_sel = '0') then
				Dec_rel(3 downto 0)	<= env_att_dec(3 downto 0);
			else
				Dec_rel(3 downto 0)	<= env_sus_rel(3 downto 0);
			end if;
		end process;

		decay_release_table:process(clk_1MHz)
		begin
			if (clk_1MHz = '1' and clk_1MHz'event) then
				if reset = '1' then
					divider_dec_rel <= 0;
				else
					case Dec_rel(3 downto 0) is
						when "0000" =>	divider_dec_rel <= 3; 		--release rate: (6mS / 1uS per clockcycle) / 1632
						when "0001" =>	divider_dec_rel <= 15; 		--release rate: (24mS / 1uS per clockcycle) / 1632
						when "0010" =>	divider_dec_rel <= 29; 		--release rate: (48mS / 1uS per clockcycle) / 1632
						when "0011" =>	divider_dec_rel <= 44; 		--release rate: (72mS / 1uS per clockcycle) / 1632
						when "0100" =>	divider_dec_rel <= 70; 		--release rate: (114mS / 1uS per clockcycle) / 1632
						when "0101" =>	divider_dec_rel <= 103; 	--release rate: (168mS / 1uS per clockcycle) / 1632
						when "0110" =>	divider_dec_rel <= 125; 	--release rate: (204mS / 1uS per clockcycle) / 1632
						when "0111" =>	divider_dec_rel <= 147; 	--release rate: (240mS / 1uS per clockcycle) / 1632
						when "1000" =>	divider_dec_rel <= 184; 	--release rate: (300mS / 1uS per clockcycle) / 1632
						when "1001" =>	divider_dec_rel <= 459; 	--release rate: (750mS / 1uS per clockcycle) / 1632
						when "1010" =>	divider_dec_rel <= 919; 	--release rate: (1500mS / 1uS per clockcycle) / 1632
						when "1011" =>	divider_dec_rel <= 1471; 	--release rate: (2400mS / 1uS per clockcycle) / 1632
						when "1100" =>	divider_dec_rel <= 1838; 	--release rate: (3000mS / 1uS per clockcycle) / 1632
						when "1101" =>	divider_dec_rel <= 5515; 	--release rate: (9000mS / 1uS per clockcycle) / 1632
						when "1110" =>	divider_dec_rel <= 9191; 	--release rate: (15000mS / 1uS per clockcycle) / 1632
						when "1111" =>	divider_dec_rel <= 14706;	--release rate: (24000mS / 1uS per clockcycle) / 1632
						when others =>	divider_dec_rel <= 0;		--
					end case;
				end if;
			end if;
		end process;

		amplitude_modulator:process(clk_1MHz)
		begin
			if (clk_1MHz = '1' and clk_1MHz'event) then
				signal_vol	<= ("00000000"& osc_out) * ("000000000000" & env_counter);		-- vysledny ton - vynasobeni vystupu z oscilatoru a vystupu z generatoru obalky (citace)
			end if;
		end process;

	end Behavioral;
