-- - - - - - - - - - - - - - - - - - - --
-- Projekt: Zaverecny projekt IVH      --
-- Autor:   Jan Stralka                --
-- Datum:   4. 6. 2011                 --
-- - - - - - - -  - - -  - - -  - -  - --

library ieee;   
use ieee.std_logic_1164.all;             
use ieee.std_logic_unsigned.all;
use IEEE.numeric_std.all;
use ieee.std_logic_arith.all;

architecture main of tlv_gp_ifc is

-- Used components:
  component SPI_adc
      generic (
         ADDR_WIDTH : integer;
         DATA_WIDTH : integer;
         ADDR_OUT_WIDTH : integer;
         BASE_ADDR  : integer
      );
      port (
         CLK      : in  std_logic;

         CS       : in  std_logic;
         DO       : in  std_logic;
         DO_VLD   : in  std_logic;
         DI       : out std_logic;
         DI_REQ   : in  std_logic;

         ADDR     : out std_logic_vector (ADDR_OUT_WIDTH-1 downto 0);
         DATA_OUT : out std_logic_vector (DATA_WIDTH-1 downto 0);
         DATA_IN  : in  std_logic_vector (DATA_WIDTH-1 downto 0);

         WRITE_EN : out std_logic;
         READ_EN  : out std_logic
      );
  end component;


  component sid6581 is
	port (
		clk40			: in std_logic;								--	main clock signal
		clk1mhz   : in std_logic;               --  1 MHz clock signal
		reset			: in std_logic;								-- high active signal (reset when reset = '1')
		cs				: in std_logic;								--	"chip select", when this signal is '1' this model can be accessed
		we				: in std_logic;								-- when '1' this model can be written to, otherwise access is considered as read

		addr			: in unsigned(4 downto 0);					-- address lines
		di				: in std_logic_vector(7 downto 0);		--	data in (to chip)
		do				: out std_logic_vector(7 downto 0);		--	data out	(from chip)
		
		audio_data	: out std_logic_vector(17 downto 0)
	);
  end component;

  
  -- for PWM  
  signal audio_out : std_logic;
  signal pwm_cnt : std_logic_vector(8 downto 0) := (others => '0');
  
  -- for SID6581 connection
  signal clk1mhz : std_logic := '0';
  signal clk_devider : std_logic_vector(39 downto 0) := "0000000000000000000011111111111111111111";
  signal sid_data_in, sid_data_out : std_logic_vector(7 downto 0) := (others => '0'); 
  signal rec_addr : std_logic_vector(4 downto 0);
  signal sid_addr : unsigned(4 downto 0);--to_unsigned(24, 5);
  signal sid_out, audio_peak : std_logic_vector(17 downto 0) := (others => '0');
  signal sid_we : std_logic := '0';

  -- for light indicator:
  signal en_100hz : std_logic := '0';
  signal leds : std_logic_vector(7 downto 0) := "00000000";
begin

   -- SPI decoder for SID
   spidecd: SPI_adc
      generic map (
         ADDR_WIDTH => 8,      -- addr width 8 b
         DATA_WIDTH => 8,      -- data width 8 b
         ADDR_OUT_WIDTH => 5,  -- output addr width for SID registers 5 b
         BASE_ADDR  => 0
      )
      port map (
         CLK      => CLK,
         CS       => SPI_CS,

         DO       => SPI_DO,
         DO_VLD   => SPI_DO_VLD,
         DI       => SPI_DI,
         DI_REQ   => SPI_DI_REQ,

         ADDR     => rec_addr,
         DATA_OUT => sid_data_in,
         DATA_IN  => sid_data_out,
         WRITE_EN => sid_we,
         READ_EN  => open
      );

  sid_addr <= to_unsigned(conv_integer(rec_addr), 5);
  
  sid6581player: sid6581
    port map (
      clk40 => CLK,
      clk1mhz => clk1mhz,
      reset => '0',
      cs => '1',
      we => sid_we,
      addr => sid_addr,
      di => sid_data_in,
      do => sid_data_out,
      audio_data => sid_out
    );

  process (CLK) -- CLK /40 devider
  begin
    if CLK'event and CLK='1' then
   	  clk_devider(39 downto 1) <= clk_devider(38 downto 0);
      clk_devider(0) <= clk_devider(39);
    end if;
  end process;

  clk1mhz <= clk_devider(39);
     
  process (CLK) -- PWM counter
  begin          
    if (CLK'event) and CLK = '1' then
      pwm_cnt <= pwm_cnt + 1; --free counter
    end if;
  end process;
   
  audio_out <= '1' when (pwm_cnt < sid_out(17 downto 9)) else '0';

  -- FITkit verze 2.x
  X(7) <= audio_out;
  X(9) <= audio_out;
  
  ledRefresh: entity work.engen
    generic map (MAXVALUE => 400000)
    port map (CLK => CLK, ENABLE => '1', EN => en_100hz);
  
  process (CLK) -- ligh indicator
  begin
    if CLK'event and CLK='1' then
      if en_100hz = '1' then
        case audio_peak(17 downto 13) is
          when "01111"|"1----" =>
            leds <= "11111111";
          when "01110" =>
            leds <= "01111111";
          when "01101" =>
            leds <= "00111111";
          when "01100" =>
            leds <= "00011111";
          when "01011" =>
            leds <= "00001111";
          when "01010" =>
            leds <= "00000111";
          when "01001" =>
            leds <= "00000011";
          when "01000" =>
            leds <= "00000001";
          when others =>
            leds <= "00000000";
        end case;
        audio_peak <= (others => '0');
      end if;
      
      if audio_peak < sid_out then
        audio_peak <= sid_out; -- saves audio peak value
      end if;
      
    end if;
  end process;

  ledgen: for i in 0 to 7 generate
    -- FITkit verze 2.x
    X(8 + 2*i) <= leds(i);
  end generate;

end main;
