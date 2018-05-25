<?php

/* Define macro constants */
define('PARAM_ERROR', 10);
define('OPEN_FILE_IN_ERROR', 11);
define('OPEN_FILE_OUT_ERROR', 12);
define('INTERNAL_ERROR', 99);
define('SYNTAX_ERROR', 21);

/* Prints error message and ends program with PARAM_ERROR */
function param_error()
{
	fwrite(STDERR, "Incorrect parameters. For help use parameter: --help\n");
	exit(PARAM_ERROR);
}

/* Prints help */
function help(){
print "Usage of script 'test.php':
 run>  php5.6 test.php   
with>     (no arguments)              [Start testing]
          --help                      [Help]
          --directory=file            [Required file is DIR With Tests]
          --recursive                 [Recursive through dirs in DIR]
          --parse-script=file         [Required file is script to analyse code .IPPcode18]
          --int-script=file           [Required file is script to interpret XML representation of .IPPcode18]\n";
    exit(0);
}

/* Long options */
$longopts  = array("help","directory:","recursive","parse-script:","int-script:");

/* Options from arguments */
$options = getopt("", $longopts);

foreach ($options as $key => $file){	
	if(count($options[$key]) > 1) /* If the same argument is there multiple times */
		param_error();
	if($key == "help" && $argc != 2) /* If some other argument with --help was passed */
		param_error();
}

if($argc-1 != count($options)) /* If count of passed arguments doesnt match the count of options */
	param_error();

/* Default options */
$dir = ".";
$recursion_on = false;
$parser = "./parse.php";
$interpret = "./interpret.py";

if($argc == 1)
{
	/* Start testing */
}
else if(isset($options["help"]) && $argc == 2)
{
	help();
}
else if($argc >= 2 && $argc <= 5)
{
	if (isset($options["directory"]))
	{
		if(file_exists($options["directory"]) && is_dir($options["directory"]))
			$dir = $options["directory"];
		else
		{
			fwrite(STDERR, "Directory passed to script doesnt exit\n");
			exit(OPEN_FILE_IN_ERROR);
		}

	}

	if(isset($options["recursive"]))
	{
		$recursion_on = true;
	}

	if (isset($options["parse-script"]))
	{
		if(file_exists($options["parse-script"]) && !is_dir($options["parse-script"]))
			$parser = $options["parse-script"];
		else
		{
			fwrite(STDERR, "Parse-script passed to script doesnt exit\n");
			exit(OPEN_FILE_IN_ERROR);
		}
	}

	if (isset($options["int-script"]))
	{
		if(file_exists($options["int-script"]) && !is_dir($options["int-script"]))
			$interpret = $options["int-script"];
		else
		{
			fwrite(STDERR, "Int-script passed to script doesnt exit\n");
			exit(OPEN_FILE_IN_ERROR);
		}
	}
}
else
{
	param_error();
}

if($recursion_on)
{
	$output = shell_exec("find $dir -name '*.src'| sort"); // !!! recursive
}
else
{
	$output = shell_exec("find $dir -maxdepth 1 -name '*.src'| sort ");
}

$files = preg_split('/\n/', $output);

/* Create file - filename.[rc|in|out] if doesnt exist */
function create_file($file)
{
	if(!file_exists("$file"))
	{
		if(!($rcfile = fopen("$file", "w")))
		{
			fwrite(STDERR, "File cant be opened!\n"); //Cant open file 
			exit(OPEN_FILE_OUT_ERROR);
		}

		if(preg_match("/\X+\.rc$/", "$file"))
			fwrite($rcfile, "0");

		if(!(fclose($rcfile)))
		{
			fwrite(STDERR, "File cant be closed!\n"); //Cant close file 
			exit(INTERNAL_ERROR);
		}
	}
	else if(is_dir("$file"))
	{
		fwrite(STDERR, "FILE: '$file' is directory !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"); 
		exit(OPEN_FILE_OUT_ERROR);
	}
}


$alltests = array();

foreach ($files as $index => $filepath) {
	if(!empty($filepath))
	{
		$dirpath = preg_split("/\/(.(?!\/))+$/", $filepath)[0];
		$filepathsplit = preg_split("/\//", $filepath);
		$filename = end($filepathsplit);
		$filename = preg_split("/\./", $filename)[0];

		$tmp = tmpfile();	
		$meta = stream_get_meta_data($tmp)['uri'];

		$tmp2 = tmpfile();	
		$meta2 = stream_get_meta_data($tmp2)['uri'];

		$parser_retcode = intval(shell_exec("php5.6 $parser < $filepath > $meta ; echo $?"),10);

		fseek($tmp, 0);

		if($parser_retcode == 0)
		{
			create_file("$dirpath/$filename.rc");
			create_file("$dirpath/$filename.in");
			create_file("$dirpath/$filename.out");

			#fwrite(STDERR, $filename);

			$interpret_retcode = intval(shell_exec("cat $dirpath/$filename.in | python3.6 $interpret --source=$meta > $meta2 ; echo $?"),10); 
			$file_retcode = intval(shell_exec("cat $dirpath/$filename.rc"),10);


			if($file_retcode == $interpret_retcode)
			{
				$diffrc = intval(shell_exec("diff $dirpath/$filename.out $meta2 ; echo $?"));

				if($diffrc == 0)
				{
					$alltests[$dirpath][$filename][] = "passed";
				}
				else
				{
					$alltests[$dirpath][$filename][] = "failed";
				}
			}
			else
			{
				$alltests[$dirpath][$filename][] = "failed";
			}

			$alltests[$dirpath][$filename]["parserrc"] = $parser_retcode;
			$alltests[$dirpath][$filename]["intrc"] = $interpret_retcode;
			$alltests[$dirpath][$filename]["expectedrc"] = $file_retcode;

		}
		else
		{
			create_file("$dirpath/$filename.rc");
			create_file("$dirpath/$filename.in");
			create_file("$dirpath/$filename.out");

			$file_retcode = intval(shell_exec("cat $dirpath/$filename.rc"),10);

			if($file_retcode == $parser_retcode)
			{
				$alltests[$dirpath][$filename][] = "passed";
			}
			else
			{
				$alltests[$dirpath][$filename][] = "failed";
			}

			$alltests[$dirpath][$filename]["parserrc"] = $parser_retcode;
			$alltests[$dirpath][$filename]["intrc"] = '-';
			$alltests[$dirpath][$filename]["expectedrc"] = $file_retcode;
		}

		fclose($tmp);
		fclose($tmp2);
	}
}

/* Add test into html representation*/
function add_test_record($xml, $dir, $file, $result)
{
	xmlwriter_start_element($xml, 'tr');

		xmlwriter_start_element($xml, 'td');
			xmlwriter_text($xml, "$file");
 		xmlwriter_end_element($xml);//td

 		xmlwriter_start_element($xml, 'td');
			xmlwriter_text($xml, $dir);
 		xmlwriter_end_element($xml);//td

 		xmlwriter_start_element($xml, 'td');
			xmlwriter_text($xml, $result['parserrc']);
 		xmlwriter_end_element($xml);//td

 		xmlwriter_start_element($xml, 'td');
			xmlwriter_text($xml, $result['intrc']);
 		xmlwriter_end_element($xml);//td

 		xmlwriter_start_element($xml, 'td');
			xmlwriter_text($xml, $result['expectedrc']);
 		xmlwriter_end_element($xml);//td

 		xmlwriter_start_element($xml, 'td');
 				xmlwriter_start_attribute($xml, 'class');
 					xmlwriter_text($xml, $result[0]);
				xmlwriter_end_attribute($xml);
 			xmlwriter_text($xml, $result[0]);
 		xmlwriter_end_element($xml);//td

 	xmlwriter_end_element($xml);//tr

}

/* Generating HTML */
$xml = xmlwriter_open_memory(); 
xmlwriter_set_indent($xml, 1); 
xmlwriter_set_indent_string($xml,'    ');

xmlwriter_start_dtd($xml,'html');
xmlwriter_end_dtd($xml);

xmlwriter_start_element($xml, 'html');
xmlwriter_start_element($xml, 'head');

xmlwriter_start_element($xml, 'title');
xmlwriter_text($xml,"IPP Automated Tests");
xmlwriter_end_element($xml);

xmlwriter_start_element($xml, 'style');
xmlwriter_text($xml,"
	.passed {color: green; font-weight: bold;}
	.failed {color: red; font-weight: bold;}
	.regular {color: black; font-weight: bold;}
	div{margin-top: 50px;}
	html{font-family: Verdana;}
	div h1{
	font-weight: bold;
	text-align: center;
	width:100%;
	margin-top: -30px;}
	hr {
		margin-top: -15px;
		border:none;
		border-top:2px solid #0066cc;
		color:#fff;
		height:1px;
		width:100%;}	
	.overall{margin-top: 50px;}
	#boldline {border-top:2px solid #0066cc;}
	.dirresult{
		background-color: #f5f5f5;
		height: 10px;	}
	.narrow {max-width: 50px;}
	.dirresult td{
		height: 40px;
		font-size: 18px;
		font-weight: bold;	}
	tr.dirresult:hover{
		background-color: #3366cc;
		color:white;}	
	tr.border-bottom td{border-bottom: 2px solid #0066cc;}	
	table.results{width: 100%; table-layout: fixed;	margin: 0 auto;	}
	.results td{width: 50px; text-align: center;}
	.results td+td{text-align: left;}
	table.tests {width: 100%; table-layout: fixed;} 
	tr:hover {
		background-color: #f5f5f5;
		font-weight: bold;}
	th,td{width: 100px;}
	th{height: 30px; text-align: center; background-color: #3366cc; color: white; font-style: strong;}
	td{text-align: center; word-wrap: break-word; white-space: normal;}
	.td_left{text-align: left;}
	td+td, th+th {text-align: center; width: 80px;}
	td+td+td, th+th+td {text-align: center; width: 40px;}

			");
 			xmlwriter_end_element($xml); //style
 	xmlwriter_end_element($xml); //head

 	xmlwriter_start_element($xml, 'body');

 		xmlwriter_start_element($xml, 'div');

 			xmlwriter_start_element($xml, 'h1');
 				xmlwriter_text($xml,"IPP Automated Tests");
 			xmlwriter_end_element($xml);//h1

 			xmlwriter_start_element($xml, 'hr');
 			xmlwriter_end_element($xml);//hr

 			xmlwriter_start_element($xml, 'table');
 				xmlwriter_start_attribute($xml, 'class');
 				xmlwriter_text($xml, 'tests');
				xmlwriter_end_attribute($xml);

				xmlwriter_start_element($xml, 'tr');

					xmlwriter_start_element($xml, 'th');
						xmlwriter_text($xml, 'File');
 					xmlwriter_end_element($xml);//th

 					xmlwriter_start_element($xml, 'th');
 						xmlwriter_text($xml, 'Directory');
 					xmlwriter_end_element($xml);//th

 					xmlwriter_start_element($xml, 'th');
 						xmlwriter_start_attribute($xml, 'class');
 						xmlwriter_text($xml, 'narrow');
						xmlwriter_end_attribute($xml);
 						xmlwriter_text($xml, 'Parser RC');
 					xmlwriter_end_element($xml);//th

 					xmlwriter_start_element($xml, 'th');
 						xmlwriter_start_attribute($xml, 'class');
 						xmlwriter_text($xml, 'narrow');
						xmlwriter_end_attribute($xml);
 						xmlwriter_text($xml, 'Interpret RC');
 					xmlwriter_end_element($xml);//th

 					xmlwriter_start_element($xml, 'th');
 						xmlwriter_start_attribute($xml, 'class');
 						xmlwriter_text($xml, 'narrow');
						xmlwriter_end_attribute($xml);
 						xmlwriter_text($xml, 'Expected RC');
 					xmlwriter_end_element($xml);//th


 					xmlwriter_start_element($xml, 'th');
 						xmlwriter_text($xml, 'Test Result');
 					xmlwriter_end_element($xml);//th


 				xmlwriter_end_element($xml);//tr

 				
 					$passed_tests_count = 0;
 					$failed_tests_count = 0;
 					$passed_tests_in_dir = 0;
 					$failed_tests_in_dir = 0;
 					$alltests_count = 0;
 					$percentage_of_success = 0;

 				if(!empty($alltests))
 				{
					foreach ($alltests as $dir => $tests) 
					{
   						foreach ($tests as $test => $result) 
   						{
   							add_test_record($xml, $dir, $test, $result);
   							if($result[0] == "passed")
   							{
   								$passed_tests_count++;
   								$passed_tests_in_dir++;
   							}
   							else
   							{
   								$failed_tests_count++;
   								$failed_tests_in_dir++;
   							}
    					}

    					xmlwriter_start_element($xml, 'tr');
    						xmlwriter_start_attribute($xml, 'class');
					 					xmlwriter_text($xml, "dirresult border-bottom");
							xmlwriter_end_attribute($xml);

							xmlwriter_start_element($xml, 'td');
								xmlwriter_text($xml, "");
					 		xmlwriter_end_element($xml);//td

					 		xmlwriter_start_element($xml, 'td');
								xmlwriter_text($xml, $dir);
					 		xmlwriter_end_element($xml);//td


							xmlwriter_start_element($xml, 'td');
							xmlwriter_start_attribute($xml, 'class');
 						xmlwriter_text($xml, 'narrow');
						xmlwriter_end_attribute($xml);
								xmlwriter_text($xml, "");
					 		xmlwriter_end_element($xml);//td

					 		xmlwriter_start_element($xml, 'td');
					 		xmlwriter_start_attribute($xml, 'class');
 						xmlwriter_text($xml, 'narrow');
						xmlwriter_end_attribute($xml);
								xmlwriter_text($xml, "");
					 		xmlwriter_end_element($xml);//td


							xmlwriter_start_element($xml, 'td');
							xmlwriter_start_attribute($xml, 'class');
 						xmlwriter_text($xml, 'narrow');
						xmlwriter_end_attribute($xml);
								xmlwriter_text($xml, "");
					 		xmlwriter_end_element($xml);//td



					 		xmlwriter_start_element($xml, 'td');
					 			$all_tests_in_dir = $passed_tests_in_dir + $failed_tests_in_dir;
					 			xmlwriter_text($xml, "$passed_tests_in_dir"."/"."$all_tests_in_dir");
					 			$passed_tests_in_dir=0;
					 			$failed_tests_in_dir=0;
					 		xmlwriter_end_element($xml);//td
					 	xmlwriter_end_element($xml);//tr


					 	xmlwriter_start_element($xml, 'tr');
    						xmlwriter_start_attribute($xml, 'class');
					 					xmlwriter_text($xml, "rowspace");
							xmlwriter_end_attribute($xml);

							xmlwriter_start_element($xml, 'td');
								xmlwriter_text($xml, "");
					 		xmlwriter_end_element($xml);//td

					 		xmlwriter_start_element($xml, 'td');
								xmlwriter_text($xml, "");
					 		xmlwriter_end_element($xml);//td

					 		xmlwriter_start_element($xml, 'td');
					 			xmlwriter_text($xml, "");
					 		xmlwriter_end_element($xml);//td
					 	xmlwriter_end_element($xml);//tr


					}

					$alltests_count = $passed_tests_count+$failed_tests_count;
					$percentage_of_success = round($passed_tests_count/$alltests_count*100);
				}


 			xmlwriter_end_element($xml);//table

 			xmlwriter_start_element($xml, 'h1');
 				xmlwriter_start_attribute($xml, 'class');
 					xmlwriter_text($xml, 'overall');
				xmlwriter_end_attribute($xml);
 				xmlwriter_text($xml,"Overall Results");
 			xmlwriter_end_element($xml);//h1 class = "overall"

 			xmlwriter_start_element($xml, 'hr');
 				xmlwriter_start_attribute($xml, 'id');
 					xmlwriter_text($xml, 'boldline');
				xmlwriter_end_attribute($xml);
 			xmlwriter_end_element($xml);//hr



 			xmlwriter_start_element($xml, 'table');
 				xmlwriter_start_attribute($xml, 'class');
 					xmlwriter_text($xml, 'results');
				xmlwriter_end_attribute($xml);


				xmlwriter_start_element($xml, 'tr');

					xmlwriter_start_element($xml, 'td');
						xmlwriter_start_attribute($xml, 'class');
 							xmlwriter_text($xml, 'td_left');
						xmlwriter_end_attribute($xml);
						xmlwriter_text($xml, 'Total Tests');

 					xmlwriter_end_element($xml);//td

 					xmlwriter_start_element($xml, 'td');
 						xmlwriter_start_attribute($xml, 'class');
 						 	xmlwriter_text($xml, 'regular');
						xmlwriter_end_attribute($xml);
 						xmlwriter_text($xml, $alltests_count);

 					xmlwriter_end_element($xml);//td

 				xmlwriter_end_element($xml);//tr



 				xmlwriter_start_element($xml, 'tr');

					xmlwriter_start_element($xml, 'td');

						xmlwriter_start_attribute($xml, 'class');
 							xmlwriter_text($xml, 'td_left');
						xmlwriter_end_attribute($xml);
						xmlwriter_text($xml, 'Passed Tests');						
 					xmlwriter_end_element($xml);//td

 					xmlwriter_start_element($xml, 'td');
 						xmlwriter_start_attribute($xml, 'class');
 							xmlwriter_text($xml, 'passed');
						xmlwriter_end_attribute($xml);
 						xmlwriter_text($xml, $passed_tests_count);
 					xmlwriter_end_element($xml);//td

 				xmlwriter_end_element($xml);//tr

 				xmlwriter_start_element($xml, 'tr');

					xmlwriter_start_element($xml, 'td');
					xmlwriter_start_attribute($xml, 'class');
 							xmlwriter_text($xml, 'td_left');
						xmlwriter_end_attribute($xml);
						xmlwriter_text($xml, 'Failed Tests');
						
 					xmlwriter_end_element($xml);//td

 					xmlwriter_start_element($xml, 'td');
 						xmlwriter_start_attribute($xml, 'class');
 							xmlwriter_text($xml, 'failed');
						xmlwriter_end_attribute($xml);
 						xmlwriter_text($xml, $failed_tests_count);
 					xmlwriter_end_element($xml);//td

 				xmlwriter_end_element($xml);//tr

 				xmlwriter_start_element($xml, 'tr');

					xmlwriter_start_element($xml, 'td');
						xmlwriter_start_attribute($xml, 'class');
 							xmlwriter_text($xml, 'td_left');
						xmlwriter_end_attribute($xml);
						xmlwriter_text($xml, 'Percentage of Passed Tests');

 					xmlwriter_end_element($xml);//td

 					xmlwriter_start_element($xml, 'td');
 						xmlwriter_start_attribute($xml, 'class');
 							xmlwriter_text($xml, 'passed');
						xmlwriter_end_attribute($xml);
 						xmlwriter_text($xml, "$percentage_of_success%");
 					xmlwriter_end_element($xml);//td

 				xmlwriter_end_element($xml);//tr


			xmlwriter_end_element($xml);//table results


 		xmlwriter_end_element($xml);//div

 	xmlwriter_end_element($xml);//body

xmlwriter_end_element($xml);//html
xmlwriter_end_document($xml); 
echo xmlwriter_output_memory($xml);

?>