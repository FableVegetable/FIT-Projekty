-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2017 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Tomas Zubrik 2017(C) xzubri00@stud.fit.vutbr.cz
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
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni z pameti (DATA_RDWR='0') / zapis do pameti (DATA_RDWR='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WE musi byt '0'
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
-- ----------------- Architecture declaration ---------------------------------
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

--------------------------------------------------------------
------------------------ FSM States  -------------------------
--------------------------------------------------------------
type fsm_state is 
(
	s_start,		-- Start 
	s_fetch,		-- Fetch
	s_decode,		-- Decode
	s_ptr_inc,		-- RAM Pointer++
	s_ptr_dec,		-- RAM Pointer--
	s_val_inc,		-- RAM[ptr]++
	s_val_inc_do,	-- *
	s_val_dec,		-- RAM[ptr]--
	s_val_dec_do,	-- *
	s_while_begin,	-- While loop begins
	s_wloop_1,    	-- *
	s_wloop_2,   	-- *
	s_wloop_3,   	-- *	
	s_while_end,	-- While loop ends
	s_wlend1, 		-- *
	s_wlend2,		-- *
	s_wlend3,		-- *
	s_wlend4,		-- *
	s_break,		-- While loop break
	s_br_con,		-- *
	s_br_con2,		-- *
	s_wchar,		-- Putchar
	s_wchar_do,		-- *
	s_rchar,		-- Getchar
	s_rchar_do,		-- *
	s_halt,			-- Finish
	s_nop			-- Skip
);

--------------------------------------------------------------
------------------- Instructions -----------------------------
--------------------------------------------------------------
type instruction is 
(
  i_ptr_inc,      -- >
  i_ptr_dec,      -- <
  i_val_inc,      -- +
  i_val_dec,      -- -
  i_while_begin,  -- [
  i_while_end,    -- ]
  i_break,    	  -- ~
  i_wchar,        -- .
  i_rchar,        -- ,
  i_halt,         -- null
  i_nop           -- others
);


--------------------------------------------------------------
------------------------ Signals -----------------------------
--------------------------------------------------------------
signal pc_addr              	: std_logic_vector(11 downto 0);        -- PC register adress value
signal pc_inc, pc_dec			: std_logic;                            -- Increments and decrements value of PC (program counter)

signal ptr_addr             	: std_logic_vector(9 downto 0);         -- PTR register adress value
signal ptr_inc, ptr_dec     	: std_logic;                            -- Increments and decrements value of PTR (pointer register)

signal cnt              		: std_logic_vector (7 downto 0);        -- CNT value
signal cnt_inc, cnt_dec,cnt_set : std_logic;                            -- Increments and decrements value of CNT (counter of brackets)

signal mem_select           	: std_logic_vector (1 downto 0);        -- Multiplexor Logic Vector

signal instr                	: instruction;                          -- Actual Instruction
signal pstate, nstate       	: fsm_state;                            -- Present State and Next State of FSM


begin

DATA_ADDR <= ptr_addr;
CODE_ADDR <= pc_addr;         
OUT_DATA  <= DATA_RDATA;

-------------------------------------------------------------
-------------------- PC Register Process --------------------
-------------------------------------------------------------
reg_pc : process(RESET, CLK)
begin
  if (RESET = '1') then
    pc_addr <= (others => '0');
  elsif (CLK = '1') and (CLK'event) then
    if (pc_inc = '1') then
      pc_addr <= pc_addr + 1;
    elsif (pc_dec = '1') then
      pc_addr <= pc_addr - 1;
    end if;
  end if;
end process reg_pc;

-------------------------------------------------------------
------------------- PTR Register Process --------------------
-------------------------------------------------------------
reg_ptr : process (RESET, CLK)
begin
  if (RESET = '1') then
    ptr_addr <= (others => '0');
  elsif (CLK = '1') and (CLK'event) then
    if (ptr_inc = '1') then
      ptr_addr <= ptr_addr + 1;
    elsif (ptr_dec = '1') then
      ptr_addr <= ptr_addr - 1;
    end if;
  end if;
end process reg_ptr;

-------------------------------------------------------------
-------------------- CNT Register Process -------------------
-------------------------------------------------------------
reg_cnt : process (RESET, CLK, cnt_set)
begin
  if (RESET = '1') then
    cnt <= (others => '0');
  elsif (CLK = '1') and (CLK'event) then
    if (cnt_inc = '1') then
      cnt <= cnt + 1;
    elsif (cnt_dec = '1') then
      cnt <= cnt - 1;
    end if;
  end if;
  if(cnt_set = '1') then -- set counter to value = 1
  	cnt <= X"01";
  end if;
end process reg_cnt;

--------------------------------------------------------------
------------------- Instruction Decoder ----------------------
--------------------------------------------------------------
instr_decode : process (CODE_DATA)
begin
    case CODE_DATA is
      when X"3E"  => instr <= i_ptr_inc;      -- >
      when X"3C"  => instr <= i_ptr_dec;      -- <
      when X"2B"  => instr <= i_val_inc;      -- +
      when X"2D"  => instr <= i_val_dec;      -- -
      when X"5B"  => instr <= i_while_begin;  -- [
      when X"5D"  => instr <= i_while_end;    -- ]
      when X"7E"  => instr <= i_break;	      -- ~
      when X"2E"  => instr <= i_wchar;        -- .
      when X"2C"  => instr <= i_rchar;        -- ,
      when X"00"  => instr <= i_halt;         -- null
      when others => instr <= i_nop;          -- others 
    end case;
end process instr_decode;

--------------------------------------------------------------
------------------- READ Select ------------------------------
--------------------------------------------------------------
mux_select  : process (mem_select, IN_DATA, DATA_RDATA)
begin
  case mem_select is
    when "00" => DATA_WDATA <= DATA_RDATA + 1;
    when "01" => DATA_WDATA <= DATA_RDATA - 1;
    when "10" => DATA_WDATA <= IN_DATA;
    when "11" => DATA_WDATA <= X"00";
    when others => null;
  end case;
end process mux_select;

--------------------------------------------------------------
------------------- Present State Process --------------------
--------------------------------------------------------------
get_pstate   : process (RESET,CLK)
begin
  if(RESET = '1') then
    pstate <= s_start;
  elsif (CLK'event) and (CLK = '1') then
    if(EN = '1') then
      pstate <= nstate;
    end if;
  end if;
end process get_pstate;


--------------------------------------------------------------
----------------------- FSM Logic ----------------------------
--------------------------------------------------------------

fsm_logic   : process (CODE_DATA, IN_DATA, DATA_RDATA, IN_VLD, OUT_BUSY, pstate, mem_select, instr, cnt)
begin

  pc_inc <= '0';    	-- PC values initialized
  pc_dec <= '0';
  
  ptr_inc <= '0';   	-- PTR values initialized
  ptr_dec <= '0';
  
  cnt_inc <= '0';   	-- CNT values initialized
  cnt_dec <= '0';
  cnt_set <= '0';

  mem_select <= "10";	-- Multiplexor initialized
  
  IN_REQ <= '0';        
  OUT_WE <= '0';
  DATA_EN <= '0';
  CODE_EN <= '0';       -- Enable Values initialized
  DATA_RDWR <= '0';
  
  
  case ( pstate ) is
    
    -- Starting state --   
    when s_start =>	nstate <= s_fetch;

    -- Fetching instruction --                          	
    when s_fetch =>   	nstate <= s_decode;	
                        CODE_EN <= '1';
    
    ------ Decoding instruction --------------------------			
    when s_decode =>				  					--
      case instr is										--
      	when i_ptr_inc      => nstate <= s_ptr_inc;     --
        when i_ptr_dec      => nstate <= s_ptr_dec;		--
        when i_val_inc      => nstate <= s_val_inc;		--
        when i_val_dec      => nstate <= s_val_dec;		--
        when i_while_begin  => nstate <= s_while_begin;	--
        when i_while_end    => nstate <= s_while_end;	--
        when i_break 	    => nstate <= s_break;		--
        when i_wchar        => nstate <= s_wchar;		--
        when i_rchar        => nstate <= s_rchar;		--
        when i_halt         => nstate <= s_halt;		--	
        when i_nop          => nstate <= s_nop;			--
      end case;											--
    ------------------------------------------------------
    
    ------ RAM pointer shifted by inc and dec --------
    when  s_ptr_inc =>	nstate <= s_fetch;			--
                        pc_inc <= '1';				--
                        ptr_inc <= '1';				--
													--  
    when  s_ptr_dec =>  nstate <= s_fetch;			--
                        pc_inc <= '1';				--	
                        ptr_dec <= '1';				--
    --------------------------------------------------
   
		       	
    ------ Value on adress DATA_ADDR inc and dec ---------		       	
    when  s_val_inc =>  nstate <= s_val_inc_do;			--
                        DATA_RDWR <= '0';				--
                        DATA_EN <= '1';                 --           
														--
    when  s_val_inc_do =>	nstate <= s_fetch;			--
                                DATA_RDWR <= '1';		--
                                DATA_EN <= '1';			--
                                mem_select <= "00";		--
                                pc_inc <= '1';			--
														--
    when  s_val_dec =>	nstate <= s_val_dec_do;			--
                        DATA_RDWR <= '0';				--
                        DATA_EN <= '1';                 --         
														--
    when  s_val_dec_do =>  	nstate <= s_fetch;			--
                                DATA_RDWR <= '1';		--
                                DATA_EN <= '1';			--
                                mem_select <= "01";		--
                                pc_inc <= '1';			--
    ------------------------------------------------------

    -------------------	While Loop begining with '[' -------------
    when s_while_begin =>   nstate <= s_wloop_1;				--
                            DATA_RDWR <= '0';					--
                            DATA_EN <= '1'; 					--
                            pc_inc <= '1'; 						--			  
																--
    when s_wloop_1 =>	if(DATA_RDATA = X"00") then				--
                              	nstate <= s_wloop_2;			--
                        	cnt_set <= '1';						--
                        else									--
                        	nstate <= s_fetch;					--
                        end if; 								--
																--
    when s_wloop_2 =>   if(cnt /= X"00") then					--
				nstate <= s_wloop_3; 							--
				CODE_EN <= '1';									--
			else												--
				nstate <= s_fetch;								--
            		end if;										--
																--
    when s_wloop_3 =>	nstate <= s_wloop_2;					--
			if(CODE_DATA = X"5B") then							--
				cnt_inc <= '1';      							--
        		elsif(CODE_DATA = X"5D") then					--
                		cnt_dec <= '1';							--
			end if;												--
			pc_inc <= '1';										--
																--
    -------------------	While Loop begining ----------------------

    -------------------	While Loop ending with ']' ---------------
    when s_while_end =>	nstate <= s_wlend1;						--
        		DATA_RDWR <= '0';     							--
			DATA_EN <= '1'; 									--
																--
    when s_wlend1 =>	if (DATA_RDATA = X"00") then			--
             			nstate <= s_fetch;						--
             			pc_inc <= '1';							--
        		else											--
             			nstate <= s_wlend2;						--
				cnt_set <= '1';									--
				pc_dec <= '1';									--
        		end if;											--
																--	
    when s_wlend2 =>	if (cnt /= X"00") then 					--
                        	nstate <= s_wlend3;  				--
				CODE_EN <= '1';									--
                        else									--
                            	nstate <= s_fetch;				--
                        end if;           						--
																--
    when s_wlend3 =>    nstate <= s_wlend4; 					--
			if (CODE_DATA = X"5B") then							--
				cnt_dec <= '1';      							--
			elsif (CODE_DATA = X"5D") then						--
                            	cnt_inc <= '1';    				--
                        end if;									--
																--
																--
    when s_wlend4 =>    nstate <= s_wlend2;						--
                        if (cnt = X"00") then					--
				pc_inc <= '1';   								--
                        else									--
				pc_dec <= '1';									--
                        end if; 								--
																--
    -------------------	While Loop ending ------------------------

    -------------------	Break ------------------------------------		
    when s_break =>	nstate <= s_br_con;							--
			cnt_set <= '1';										--
                	pc_inc <= '1'; 								--
																--
    when s_br_con =>	if(cnt /= X"00") then					--
				CODE_EN <= '1';									--
				nstate <= s_br_con2;							--
			else												--
				nstate <= s_fetch;								--
			end if;												--
																--
    when s_br_con2 =>	nstate <= s_br_con;						--
			if (CODE_DATA = X"5B") then							--
		    		cnt_inc <= '1';      						--
			elsif (CODE_DATA = X"5D") then						--
                    		cnt_dec <= '1';    					--
                	end if;										--
			pc_inc <= '1';										--
    --------------------------------------------------------------
    
    --------- Write char on LCD display ------------------			
    when s_wchar =>	if(OUT_BUSY = '1') then				--
                       		nstate <= s_wchar;  		--
                        else							--
                               	nstate <= s_wchar_do;  	--
                        	DATA_RDWR <= '0';			--
                               	DATA_EN <= '1';			--
                        end if;							--
														--
    when s_wchar_do =>	nstate <= s_fetch;    			--
                        OUT_WE <= '1';					--
                        pc_inc <= '1';					--
    ------------------------------------------------------

    --------- Read char from keyboard --------------------			
    when s_rchar =>	IN_REQ <= '1';						--
                        if(IN_VLD = '0') then			--
                        	nstate <= s_rchar;			--
                        else							--
                        	nstate <= s_rchar_do;		--
                        end if;							--
														--
    when s_rchar_do =>	nstate <= s_fetch;				--
                        DATA_RDWR <= '1';				--
                        DATA_EN <= '1';					--
                        mem_select <= "10";				--
                        pc_inc <= '1';                  --     
    ------------------------------------------------------
    
    -- Halt instruction = End of program			
    when s_halt =>	nstate <= s_halt;

    -- Unknown instruction = Skip
    when s_nop =>	nstate <= s_fetch;
                       	pc_inc <= '1';
                                           
  end case;                    
end process fsm_logic;

end behavioral;
 
