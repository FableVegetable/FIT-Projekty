---------
--Projekt 1 z INP - Rizeni maticoveho displaye pomoci FPGA
--Autor:	Tomáš Zubrik	xzubri00
---------

--Knihovny--
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

--Rozhrani--
entity ledc8x8 is
port ( 
	ROW, LED		: out std_logic_vector (0 to 7);
 	RESET, SMCLK	: in std_logic
);
end ledc8x8;

--Architektura--
architecture main of ledc8x8 is
	signal leds, rows, counter	: std_logic_vector(7 downto 0); -- signaly
	signal ce: std_logic;

begin

--Citac--
	generate_ce: process(RESET, SMCLK)
	begin
		if RESET = '1' then -- vynuluj counter ak je nastaveny RESET na log "1"
			counter <= X"00";
		elsif SMCLK'event and SMCLK = '1' then -- s nastupnou hranou hod.signalu inkrementuj counter o 1
			counter <= counter + 1;
		end if;
	end process generate_ce;
	ce <= '1' when counter = X"FF" else '0';
	
--Register--
	rotate: process(RESET, SMCLK, ce)
	begin	
		if RESET = '1' then -- ak je RESET na log "1" do rows daj vychozi hodnotu
			rows <= "10000000";
		elsif SMCLK'event and SMCLK = '1' and ce = '1' then
			rows <= rows(0) & rows(7 downto 1); --posun jednotky doprava
		end if;
	end process rotate;

--Dekoder--
	decode: process (rows)
	begin
		case rows is -- v zavislosti od hodnoty rows prirad leds hodnotu v riadku (spolu vytvoria inicialy)
			when "10000000" => leds <= "00000111";
			when "01000000" => leds <= "11011111";
			when "00100000" => leds <= "11010000";
			when "00010000" => leds <= "11011110";
			when "00001000" => leds <= "11011101";
			when "00000100" => leds <= "11011011";
			when "00000010" => leds <= "11110111";
			when "00000001" => leds <= "11110000";
			when others =>     leds <= "11111111";
		end case;
	end process decode;	
	
--Vysledny proces zasvietenia lediek
	ROW <= rows;
	LED <= leds;

end main;
