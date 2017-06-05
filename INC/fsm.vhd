-- fsm.vhd: Finite State Machine
-- Author(s): Tomáš Zubrik
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
   type t_state is (	T1, T2, T3, T4, T5, T6, T7, 
                      TA8, TA9, TA10, TB8, TB9, TB10,
						          PRINT_OK, PRINT_ERROR, CONFIRM, DENY, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= T1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- xzubri00 : kod1 = 1150684429 (A) 	 kod2 = 1150686644 (B)
-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
	when T1 =>
		next_state <= T1;
		if (KEY(1) = '1') then
			next_state <= T2;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
    elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;     
   end if;
	  
	when T2 =>
		next_state <= T2;
		if (KEY(1) = '1') then
			next_state <= T3;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
    elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;   
		end if; 
	    
	when T3 =>
		next_state <= T3;
		if (KEY(5) = '1') then
			next_state <= T4;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
   elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;     
		end if;	  
	  
	when T4 =>
		next_state <= T4;
		if (KEY(0) = '1') then
			next_state <= T5;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
  elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;    
		end if;
      
	 when T5 =>
	   next_state <= T5;
	   if (KEY(6) = '1') then
			next_state <= T6;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
   elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;     
		end if;
		
    when T6 =>
		next_state <= T6;
		if (KEY(8) = '1') then
			next_state <= T7;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
   elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;    
		end if;	

	when T7 =>
		next_state <= T7;
		if (KEY(4) = '1') then
			next_state <= TA8;
		elsif(KEY(6) = '1') then
			next_state <= TB8;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
   elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;    
		end if;

	when TA8 =>
		next_state <= TA8;
		if (KEY(4) = '1') then
			next_state <= TA9;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
  elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;      
		end if;	
		
	when TA9 =>
		next_state <= TA9;
		if (KEY(2) = '1') then
			next_state <= TA10;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
    elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;      
		end if;	
		
	when TA10 =>
		next_state <= TA10;
		if (KEY(9) = '1') then
			next_state <= CONFIRM;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
    elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;   
		end if;	
		
	when TB8 =>
		next_state <= TB8;
		if (KEY(6) = '1') then
			next_state <= TB9;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
   elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;     
		end if;	
		
	when TB9 =>
		next_state <= TB9;
		if (KEY(4) = '1') then
			next_state <= TB10;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
   elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;     
		end if;	
		
	when TB10 =>
		next_state <= TB10;
		if (KEY(4) = '1') then
			next_state <= CONFIRM;
		elsif(KEY(15) = '1') then
			next_state <= PRINT_ERROR;
   elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;      
		end if;		
		
	when CONFIRM =>
		next_state <= CONFIRM;
		if (KEY(15) = '1') then
			next_state <= PRINT_OK;
    elsif(KEY(14 downto 0) /= "000000000000000") then
      next_state <= DENY;
		end if;
    
  when DENY =>
		next_state <= DENY;
		if (KEY(15) = '1') then
			next_state <= PRINT_ERROR;
		end if;  
		
   -- - - - - - - - - - - - - - - - - - - - - - -
   
   when PRINT_ERROR =>
      next_state <= PRINT_ERROR;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
	  
   when PRINT_OK =>
      next_state <= PRINT_OK;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;  
	  
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= T1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= T1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
     when PRINT_ERROR =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
	  
	 when PRINT_OK =>
	  FSM_MX_MEM  	 <= '1';
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
    if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;     
   end case;
end process output_logic;

end architecture behavioral;

