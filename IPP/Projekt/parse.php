<?php
/* Set internal encoding to UTF-8 */
mb_internal_encoding( "UTF-8" );

/* Define macro constants */
define('PARAM_ERROR', 10);
define('OPEN_FILE_IN_ERROR', 11);
define('OPEN_FILE_OUT_ERROR', 12);
define('INTERNAL_ERROR', 99);
define('SYNTAX_ERROR', 21);

/**
 * @brief Function help() prints short usage of script
 */ 
function help()
{
	print "Usage of script 'parse.php':
 php5.6 parse.php
          (no arguments)                  [Syntax and Lexical Analysis]
          --help                          [Help]
          --stats=file                    [Stats]
          --stats=file --loc              [Stats LOC]
          --stats=file --comments         [Stats Comments]
          --stats=file --loc --comments   [Stats LOC + Comments]\n";
    exit(0);
}

/* Function param_error() prints error message and ends program with PARAM_ERROR */
function param_error()
{
	fwrite(STDERR, "Incorrect parameters. For help use parameter: --help\n");
	exit(PARAM_ERROR);
}

/* Function syntax_error() prints error message with number of line where error is and ends program with SYNTAX_ERROR */
function syntax_error($linecount, $str)
{
	fwrite(STDERR, "Syntax error on line $linecount - $str\n");
	exit(SYNTAX_ERROR);
}

/**
 * @brief Checks argument '--stats' format.
 * @param $argv     Argument list.
 * @param $argc   	Argument count.
 * @param $index   	Index of argument in argv list.
 * @return 0 - OK, -1 - ERROR
 */
function check_stats_arg($argv, $argc, $index)
{
	GLOBAL $filename;
	if(preg_match("/^--stats=\X+/", $argv[$index]))
	{
		$filename = preg_split("/=/", $argv[$index]);

		if(preg_match("\"\X+\"", $filename[1]))
		{
			$filename = preg_split("/\"/", $filename[1]);
			$filename = preg_split("/\"/", $filename[0]);
		}	
	}
	else
		param_error();
}

/* Static flags variables */
STATIC $filename;
STATIC $flag_comm_count = false;
STATIC $flag_loc_count = false;
STATIC $comment_first = false;

/**
 * @brief Function check_args() checks check is all input argumetns are correct and sets relevant flags
 * @param $argv     Argument list.
 * @param $argc   	Argument count.
 * @return 0 - OK, -1 - ERROR
 */
function check_args($argv, $argc)
{
	GLOBAL $flag_comm_count;
	GLOBAL $flag_loc_count;
	GLOBAL $comment_first;

	if($argc ==1)
		;
	else if($argc == 2)
	{
		if($argv[1] == "--help") /* one arguments --help */
			help();
		else
			check_stats_arg($argv, $argc,1);
	}
	else if($argc == 3)
	{
		if($argv[1] == "--comments") 		/* --comments --stats */
		{
			check_stats_arg($argv, $argc, 2);
			$flag_comm_count = true;
		}
		else if($argv[1] == "--loc") 		/* --loc --stats */
		{
			check_stats_arg($argv, $argc, 2);
			$flag_loc_count = true;
		}	
		else if($argv[2] == "--comments") 	/* --stats --comments */
		{
			check_stats_arg($argv, $argc, 1);
			$flag_comm_count = true;
		}
		else if($argv[2] == "--loc") 		/*--stats --loc */
		{
			check_stats_arg($argv, $argc, 1);
			$flag_loc_count = true;
		}
		else
			param_error();
	}
	else if($argc == 4)
	{
		if((($argv[1] == "--comments" && $argv[2] == "--loc"))||
		   (($argv[2] == "--comments" && $argv[1] == "--loc")))
		{		
			check_stats_arg($argv, $argc, 3);
			if($argv[1] == "--comments")
				$comment_first = true;
		}		
		else if((($argv[1] == "--comments" && $argv[3] == "--loc"))||
				(($argv[3] == "--comments" && $argv[1] == "--loc")))
		{	
			check_stats_arg($argv, $argc, 2);
			if($argv[1] == "--comments")
				$comment_first = true;	
		}
		else if((($argv[2] == "--comments" && $argv[3] == "--loc"))||
				(($argv[3] == "--comments" && $argv[2] == "--loc")))
		{
			check_stats_arg($argv, $argc, 1);
			if($argv[2] == "--comments")
				$comment_first = true;
		}
		else
			param_error();

		$flag_comm_count = true;
		$flag_loc_count = true;		
	}
	else
		param_error();
}

/**
 * @brief Chooses value and type of instruction argument. If it doesnt match the pattern ends with syntactic error.
 * @param $str     		String.
 * @param &$type   		Address of type.
 * @param &$value   	Address of value.
 * @param $linecount   	Line number.
 */
function choose_atr_value($str, &$type, &$value, $linecount)
{
	$int_const = '/^int@((\+|\-)?\d+)$/';
	$bool_const = '/^bool@(true|false)$/';
	$string_const = '/^string@([^#\s\x5C]|\x5C[0-9]{3})*$/';
	$var = '/(LF|TF|GF)@([[:alpha:]]|_|-|\$|&|%|\*)(\w|_|-|\$|&|%|\*)*$/';

	if(preg_match($int_const, $str))
	{
		$type = "int";
		$value = explode("@", $str, 2);
		$value = $value[1];
	}
	else if(preg_match($bool_const, $str))
	{
		$type = "bool";
		$value = explode("@", $str, 2);
		$value = $value[1];
	}
	else if(preg_match($string_const, $str))
	{
		$type = "string";
		$value = explode("@", $str, 2);
		$value = $value[1];
	}
	else if(preg_match($var, $str))
	{
		$type = "var";
		$value = $str;
	}
	else
	{
		syntax_error($linecount, "Argument of instruction is invalid!");
	}
}

/**
 * @brief Adds XML element - argument into XML representation.
 * @param $type   		Type.
 * @param $value   		Value.
 * @param $argorder   	Argument order .
 */
function add_xml_arg($type, $value, $argorder)
{
	GLOBAL $xml;
	xmlwriter_start_element($xml,$argorder);
	xmlwriter_start_attribute($xml,'type');
	xmlwriter_text($xml,$type);
	xmlwriter_end_attribute($xml);
	xmlwriter_text($xml, $value);
	xmlwriter_end_element($xml); // ends arg1
}


/**
 * @brief Checks all arguments in .IPPcode18 code lines
 * @param $key     		Key of pattern.
 * @param $arguments   	Input arguments.
 * @param $linecount   	Line number.
 */
function check_regex($key, $arguments, $linecount)
{
	/* Regular expressions as patterns to check arguments*/
	$var_pattern = '/^(LF|TF|GF)@([[:alpha:]]|_|-|\$|&|%|\*)([[:alnum:]]|_|-|\$|&|%|\*)*$/';
	$label_pattern = '/^([[:alpha:]]|_|-|\$|&|%|\*)(\w|_|-|\$|&|%|\*)*$/';
	$symb_pattern ='/((LF|TF|GF)@([[:alpha:]]|_|-|\$|&|%|\*)(\w|_|-|\$|&|%|\*)*$)|(int@((\+|\-)?\d+)$|(bool@(true|false)$)|(string@(([^#|\s\\)])*(\\\d\d\d))*(([^#|\s\\)])*))$)/';
	$type_pattern ='/^int$|^string$|^bool$/';

	$int_const = '/int@(\+|\-)?\d+$/';
	$bool_const = '/bool@(true|false)$/';
	$string_const = '/(string@(([^#|\s\\)])*(\\\d\d\d))*(([^#|\s\\)])*))$/';

	switch($key){
		case "v": 	$pattern = $var_pattern; 
					$type = "var"; $value = $arguments[1];
					break;

		case "l": 	$pattern = $label_pattern; 
					$type = "label"; $value = $arguments[1];
					break;

		case "s": 	$pattern = $symb_pattern; 
					$type; $value; choose_atr_value($arguments[1], $type, $value, $linecount);
					break;

		case "vs": 	$pattern1 = $var_pattern; 
					$pattern2 = $symb_pattern; 
					$type1 = "var";	$value1 = $arguments[1];
					$type2; $value2; choose_atr_value($arguments[2], $type2, $value2, $linecount);
					break;

		case "vt": 	$pattern1 = $var_pattern; 
					$pattern2 = $type_pattern; 
					$type1 = "var";	$value1 = $arguments[1];
					$type2 = "type"; $value2 = $arguments[2];
					break;

		case "vss": $pattern1 = $var_pattern; 
					$type1 = "var";	$value1 = $arguments[1];
					$type2; $value2; choose_atr_value($arguments[2], $type2, $value2, $linecount);
					$type3; $value3; choose_atr_value($arguments[3], $type3, $value3, $linecount);
					break;

		case "lss": $pattern1 = $label_pattern; 
					$type1 = "label"; $value1 = $arguments[1];
					$type2; $value2; choose_atr_value($arguments[2], $type2, $value2, $linecount);
					$type3; $value3; choose_atr_value($arguments[3], $type3, $value3, $linecount);
					break;

		default:break;
	}

	GLOBAL $xml;

		if(strlen($key) == 1){

			add_xml_arg($type, $value, "arg1");

			if(empty($arguments[1]))
				syntax_error($linecount, "First argument of instruction is missing!");

			if(!preg_match($pattern, $arguments[1]))
				syntax_error($linecount, "First argument of instruction is invalid!");

			if(!empty($arguments[2]))
    			syntax_error($linecount, "Opcode with 1 arg, has 2 args!");
		}
		else if(strlen($key) == 2){

			add_xml_arg($type1, $value1, "arg1");
			add_xml_arg($type2, $value2, "arg2");

			if(empty($arguments[1])||empty($arguments[2]))
				syntax_error($linecount, "Instruction has 2 args <var> and <symb> - Something is missing!");

			if(!preg_match($pattern1, $arguments[1]))
			{
				syntax_error($linecount, "First argument of instruction is invalid!");
			}

			if(!preg_match($pattern2, $arguments[2]))
				syntax_error($linecount, "Second argument of instruction is invalid!");

			if(!empty($arguments[3]))
    			syntax_error($linecount, "Opcode with 1 arg, has 2 args!");
		}
		else if(strlen($key) == 3){

			add_xml_arg($type1, $value1, "arg1");
			add_xml_arg($type2, $value2, "arg2");
			add_xml_arg($type3, $value3, "arg3");

			if(empty($arguments[1])||empty($arguments[2])||empty($arguments[3]))
				syntax_error($linecount, "Instruction has 3 args <var> <symb1> <symb2> - Something is missing!");

			if(!preg_match($pattern1, $arguments[1]))
				syntax_error($linecount, "First argument of instruction is invalid!");

			if(!preg_match($symb_pattern, $arguments[2]))
				syntax_error($linecount, "Second argument of instruction is invalid!");

			if(!preg_match($symb_pattern, $arguments[3]))
				syntax_error($linecount, "Third argument of instruction is invalid!");

			if(!empty($arguments[4]))
    			syntax_error($linecount, "Opcode with 1 arg, has 2 args!");
		}
		else
			if(!empty($arguments[1]))
				syntax_error($linecount, "Opcode with no args, has args!");
}

/* Array of instruction names - opcode names, with their value as input arguments*/
$keywords = array("createframe"=>"", "pushframe"=>"", "popframe"=>"", "return"=>"", "break"=>"", "defvar"=>"v", "call"=>"l","pushs"=>"s", "pops"=>"v","write"=>"s", "label"=>"l", "jump"=>"l","dprint"=>"s", "move"=>"vs","int2char"=>"vs", "read"=>"vt","strlen"=>"vs", "type"=>"vs", "add"=>"vss", "sub"=>"vss", "mul"=>"vss", "idiv"=>"vss", "lt"=>"vss", "gt"=>"vss", "eq"=>"vss", "and"=>"vss", "or"=>"vss", "not"=>"vs", "stri2int"=>"vss", "concat"=>"vss", "getchar"=>"vss", "setchar"=>"vss", "jumpifeq"=>"lss", "jumpifneq"=>"lss");

/* Check input arguments */
check_args($argv, $argc);

/* Static variables initialization */
STATIC $flag_ippcode_comes = false;
STATIC $linecount=0;
STATIC $comment_count=0;
STATIC $loc_count=0;

/* Initialization of XML */
$xml = xmlwriter_open_memory(); 
xmlwriter_set_indent($xml, 1); 
xmlwriter_set_indent_string($xml,'    '); //4 spaces
xmlwriter_start_document($xml, '1.0', 'UTF-8');

/* Start reading lines from stdin */
while($line = fgets(STDIN))
{
	$linecount++;
	$line = trim($line); /* Cut whitespaces from beg and end */
	

	if(empty($line) || preg_match('/^\s+$/', $line)) /* If line is empty */
	{
		continue;
	}
	else if(preg_match('/(^#+\X*$)|(^\s+#+\X*$)/', $line)) /* If line is comment */
	{
		$comment_count++;
	}
	else if(preg_match("/^.ippcode18\s*(#*|#+\X*)\s*$/i", $line)) /* If line is header .IPPcode18 */
	{
		/* Adds first XML element program */
		xmlwriter_start_element($xml, 'program'); 
		xmlwriter_start_attribute($xml, 'language');
		xmlwriter_text($xml, 'IPPcode18');
		xmlwriter_end_attribute($xml);

    	if($flag_ippcode_comes == true) /* If header .IPPcode18 has been defined already */
    	{
    		syntax_error($linecount,"IPPcode18 redefined");
    	}

    	$flag_ippcode_comes = true; /* Set flag to true .IPPcode18 is defined */
    }
    else
    {
    	if($flag_ippcode_comes == false) /* If head .IPPcode18 hasnt been defined */
    	{
    		syntax_error($linecount,"IPPcode18 non defined");
    	}
    	else
    	{
    		if(preg_match('/#/', $line)) /* Increment comments count */
    			$comment_count++;

    		$linesplit = preg_split('/#/', $line); /* Split line on two parts - comment and the rest */
    		$linesplit = preg_split('/\s+/', $linesplit[0]); /*Split the rest by whitespaces */
    		$opcode = strtolower($linesplit[0]); 
  		
    		$opcodefound = false;

    		foreach ($keywords as $keyword => $key) /* Check if instruction is in keywords array*/
    		{
    			if($keyword == $opcode)
    			{
    				$loc_count++; /* Increment LOC count */
    				$opcodefound = true;

    				/* Adds XML element instruction */
    				xmlwriter_start_element($xml, 'instruction');
 					xmlwriter_start_attribute($xml, 'order');
					xmlwriter_text($xml, $loc_count);
					xmlwriter_end_attribute($xml);
					xmlwriter_start_attribute($xml, 'opcode');
					xmlwriter_text($xml, strtoupper($opcode));
					xmlwriter_end_attribute($xml);
    				break;
    			}
    		}

    		if($opcodefound == false) /* If opcode hasnt been found */
    			syntax_error($linecount, "Unknown Instruction");
    		else
    		{
    			check_regex($key, $linesplit, $linecount); /* Check correctness of arguments */
    			xmlwriter_end_element($xml);
    			$opcodefound = false;
    		}
    	}
    }
}

if($flag_ippcode_comes == false)
	syntax_error($linecount, "IPPcode18 non defined");

if(!(($argc == 1)||($argv[1] == "--help")))
{
	$path = preg_split("/\/(.(?!\/))+$/", $filename[0]); /* Get path to file */
	if(!file_exists($path[0]) && !is_dir($path[0]))
	{
		fwrite(STDERR, "File cant be opened! Path doesnt exist!\n");
		exit(OPEN_FILE_OUT_ERROR);
	}
	
	if($myfile = fopen($filename[0], "w")) /* Open file for writing stats information about LOC and comments */
	{
		if($comment_first == true) /* comments-loc in order */
		{
			fwrite($myfile, "$comment_count\n");
			fwrite($myfile, "$loc_count\n");
		}
		else
		{
			if($flag_loc_count == true && $flag_comm_count == true) /* loc-comments in order */
			{
				fwrite($myfile, "$loc_count\n");
				fwrite($myfile, "$comment_count\n");
			}
			else if($flag_comm_count == true) /* comments */
			{
				fwrite($myfile, "$comment_count\n");
			}
			else if($flag_loc_count == true) /* loc */
			{
				fwrite($myfile, "$loc_count\n");
			}
		}
	}
	else
	{
		fwrite(STDERR, "File cant be opened!\n"); /*Cant open file */
		exit(OPEN_FILE_OUT_ERROR);
	}
	
	if(!fclose($myfile))
	{
		fwrite(STDERR, "File cant be closed!\n"); /*Cant close file */
		exit(INTERNAL_ERROR);
	}
}

xmlwriter_end_element($xml);/*ends program*/
xmlwriter_end_document($xml); /*ends document*/

/* Print XML representation of .IPPcode18 */
echo xmlwriter_output_memory($xml);
?>