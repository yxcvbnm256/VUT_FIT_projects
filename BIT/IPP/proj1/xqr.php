<?php

#XQR:xpawlu00
/* Trida reprezentujici vystupni soubor. Obsahuje informace o tom, co presne chceme vyhledavat (tzn. zpracovanou query)
a vyhledavaci rekurzivni funkce. */
class XMLout
{
	public $output; 
	public $element;
	public $limit;
	public $selected = 0;
	public $whereNot = false;
	public $whereElem;
	public $whereOper;
	public $whereLiteral;
	public $from;
	/* Funkce, ktera zkopiruje cely element i s podelementy (simpleXML to neumi, proto je nutne pouzit DOM) */
	public function sxml_append(SimpleXMLElement $to, SimpleXMLElement $from)
	{
	    $toDom = dom_import_simplexml($to);
	    $fromDom = dom_import_simplexml($from);
	    $toDom->appendChild($toDom->ownerDocument->importNode($fromDom, true));
	}
	/* Funkce, ktera overuje, zda je splnena podminka WHERE u nalezeneho elementu. U stringu je nutno navic overit, zda
	je v uvozovkach. */
	public function whereCase($node)	
	{				
		if ($node->Count() > 0)
		{
			fwrite(STDERR, "WHERE element ma dalsi podelementy!\n");
			die(4);
		}	
		switch ($this->whereOper)
		{
			case "=":
				if (!is_numeric($this->whereLiteral) && strpos($this->whereLiteral, '"') === 0)
				{
					/* Overeni a zaroven zbaveni se uvozovek ve stringu */
					$literal = preg_replace("/^\"/","",$this->whereLiteral);
					if (strpos($literal, '"') === (strlen($literal)-1))
					{
						$literal = preg_replace("/\"$/","",$literal);
						if (strcmp($node->__toString(), $literal) == 0) return true;
						return false;	
					}
					else
					{
						fwrite(STDERR, "Ocekavan string za =\n");
						die(80);
					}
				}
				/* Literal string, ale neobsahuje na zacatku uvozovky */
				else if (!is_numeric($this->whereLiteral))
				{
					fwrite(STDERR, "Ocekavan string za =\n");
					die(80);
				}
				/* Literal je integer */
				else 
				{							
					if ($node->__toString() == $this->whereLiteral) return true;
				}
				break;			
			case ">":
				if (!is_numeric($this->whereLiteral) && strpos($this->whereLiteral, '"') === 0)
				{
					/* Overeni a zaroven zbaveni se uvozovek ve stringu */
					$literal = preg_replace("/^\"/","",$this->whereLiteral);
					if (strpos($literal, '"') === (strlen($literal)-1))
					{
						$literal = preg_replace("/\"$/","",$literal);
						if (strcmp($node->__toString(), $literal) > 0) return true;
					}
					else
					{
						fwrite(STDERR, "Ocekavan string za >\n");
						die(80);
					}
				}
				/* Literal string, ale neobsahuje na zacatku uvozovky */
				else if (!is_numeric($this->whereLiteral))
				{
					fwrite(STDERR, "Ocekavan string za >\n");
					die(80);
				}
				/* Literal je integer */
				else 
				{	
					if ($node->__toString() > $this->whereLiteral) return true;
				}
				break;			
			case "<":
				if (!is_numeric($this->whereLiteral) && strpos($this->whereLiteral, '"') === 0)
				{
					/* Overeni a zaroven zbaveni se uvozovek ve stringu */
					$literal = preg_replace("/^\"/","",$this->whereLiteral);
					if (strpos($literal, '"') === (strlen($literal)-1))
					{
						$literal = preg_replace("/\"$/","",$literal);
						if (strcmp($node->__toString(), $literal) < 0) return true;
					}
					else
					{
						fwrite(STDERR, "Ocekavan string za <\n");
						die(80);
					}
				}
				/* Literal string, ale neobsahuje na zacatku uvozovky */
				else if (!is_numeric($this->whereLiteral))
				{
					fwrite(STDERR, "Ocekavan string za <\n");
					die(80);
				}
				/* Literal je integer */
				else 
				{							
					if ($node->__toString() < $this->whereLiteral) return true;
				}
				break;			
			case "CONTAINS":
				if (strpos($this->whereLiteral, '"') === 0)
				{
					/* Overeni a zaroven zbaveni se uvozovek ve stringu */
					$literal = preg_replace("/^\"/","",$this->whereLiteral);
					if (strpos($literal, '"') === (strlen($literal)-1))
					{
						$literal = preg_replace("/\"$/","",$literal);
						if (strpos($node, $literal) !== FALSE) return true;
					}
					else
					{
						fwrite(STDERR, "Ocekavan string za CONTAINS\n");
						die(80);
					}
				}
				else
				{
					fwrite(STDERR, "Ocekavan string za CONTAINS\n");
					die(80);
				}
				break;
		}
		return false;
	}
	/* Funkce, ktera hleda element, element.attribute nebo .attribute WHERE. Kdyz je potreba, zanori se do podelementu. */	
	public function verifyWhere($node)
	{
			/* Pokud je zadan pouze element (explode podle tecky - pouze prvni prvek pole) */
			if (!isset($this->whereElem[1]))
			{
				if ($node->getName() == $this->whereElem[0])
				{
					if ($this->whereCase($node))	return true;					
				}
				else 
				{
					foreach ($node->children() as $child)
					{
						$gg = $this->verifyWhere($child);
						if ($gg == true) return true;
					}
				}
			}
			/* Pokud je zadan pouze .attibute (explode podle tecky - prvni prvek pole je prazdny) */
			else if ($this->whereElem[0] == "")
			{
				foreach($node->attributes() as $attr => $value)
				{					
					if ($attr == $this->whereElem[1])
					{
						if ($this->whereCase($value))	return true;
					}					
				}
				if ($node->count() > 0)
				{
					foreach($node->children as $child)
					{
						$gg = $this->verifyWhere($child);
						if ($gg == true) return true;
					}
				}
			}
			/* Pokud je zadan element.attibute (explode podle tecky - oba dva prvky pole existuji) */
			else
			{
				if ($node->getName() == $this->whereElem[0])
				{
					foreach($node->attributes() as $attr => $value)
					{		
						if ($attr == $this->whereElem[1])
						{
							if ($this->whereCase($value))	return true;
						}					
					}
				}
				else 
				{
					foreach ($node->children() as $child)
					{
						$gg = $this->verifyWhere($child);
						if ($gg == true) return true;
					}
				}
			}		
	}
	/* Funkce, ktera hleda element SELECT, jako parametr dostava element FROM, ve kterem se bude hledat. */
	public function searchXML($node)
	{
		if ($node != NULL)
		{
			/* Prohledava vsechny prvky na stejne urovni */
			foreach($node as $child)
			{
				if ($child->getName() == $this->element)
				{
					if (isset($this->limit) && $this->selected >= $this->limit)
						break;
					else
					{
						/* Pokud je zadana podminka WHERE, musi se overit jeji platnost pred kopii elementu do vystupniho souboru */
						if (isset($this->whereElem) && isset($this->whereLiteral) && isset($this->whereOper))
						{
							$bool = $this->verifyWhere($child);
							if (($bool == true && $this->whereNot == false) || ($bool == false && $this->whereNot==true))
							{	
								$this->sxml_append($this->output,$child);
								$this->selected++;
							}
						}
						else
						{
							$this->sxml_append($this->output,$child);
							$this->selected++;
						}
					}
				}
				/* Pokud neni aktualni element spravny, skript se zanori do jeho podelementu */
				else if ($child->count() > 0)
				{
					$this->searchXML($child->children());
				}
			}
		}
	}
	/* Vyhledani elementu FROM. Jako parametr funkce dostane cele XML */
	public function searchFrom($node)
	{		
		if ($this->from[0] == 'ROOT') return $node;	
		if ($this->from[0] == $node->getName()) return $node;
		foreach($node as $child)
		{		
			/* Pokud je FROM element (explode podle tecky - pouze element) */	
			if (!isset($this->from[1]))
			{					
				if ($child->getName() == $this->from[0]) return $child;
				/* Pokud neni tento prvek hledanym prvkem a ma nejake podelementy, funkce se zanori */
				else if ($child->count() > 0)
				{
					$gg = $this->searchFrom($child->children());
					if ($gg != NULL) return $gg;
				}
			}
			/* Pokud je FROM .attribute (explode podle tecky - pouze atribut) */
			else if ($this->from[0] == "")
			{				
				foreach($child->attributes() as $attr => $value)
				{
					
					if ($attr == $this->from[1]) return $child;
				}
				if ($child->count() > 0)
				{
					$gg = $this->searchFrom($child->children());
					if ($gg != NULL) return $gg;
				}
			}
			/* FROM je element.attribute */
			else
			{
				if ($child->getName() == $this->from[0])
				{					
					foreach($child->attributes() as $attr => $value)
					{						
						if ($attr == $this->from[1]) return $child;
					}
				}
				else if ($child->count() > 0)
				{
					$gg = $this->searchFrom($child->children());
					if ($gg != NULL) return $gg;
				}
			}				
		}	
	}
}
/* Trida reprezentujici vstupni XML. Obsahuje informace o parametrech a take overuje query. */
class XMLin
{
	protected $inputfilename;
	public $outputfilename;
	protected $query;
	protected $queryfile;
	public $noHeader;
	public $inputxml;
	public $root;
	/* Funkce, ktera overuje argumenty a uklada informace do promennych objektu. */
	public function decodeArguments($argv,$argc)
	{
		foreach ($argv as $arg)
		{
			if ((strpos($arg,"--input=") === 0) && (!isset($this->inputfilename)))
			{	
				$e = explode("=",$arg,2);
				$this->inputfilename = $e[1];				
			}
			else if (strpos($arg,"--output=") === 0 && !isset($this->outputfilename))
			{	
				$e = explode("=",$arg,2);			
				$this->outputfilename = $e[1];
			}
			else if (strpos($arg,"--query=") === 0 && !isset($this->query) && !isset($this->queryfile))
			{				
				$e = explode("=",$arg,2);
				$this->query = $e[1];				
			}
			else if (strpos($arg,"--qf=") === 0 && !isset($this->queryfile) && !isset($this->query))
			{				
				$e = explode("=",$arg,2);		
				$this->queryfile = file_get_contents($e[1]);				
			}
			else if ($arg == "-n" && !isset($this->noHeader))
			{				
				$this->noHeader = true;
			}
			else if (strpos($arg,"--root=") === 0 && !isset($this->root))
			{
				$e = explode("=",$arg,2);
				$this->root = $e[1];
			}
			else if ($arg == "--help" && $argc == 2)
			{
				echo "Napoveda:
				--help zobrazi napovedu
				--input nastavi vstupni soubor
				--output nastavi vystupni soubor
				--root nastavi korenovy element
				--query nebo --qf nastavi query, resp. soubor obsahujici query (musi byt zadan prave jeden z nich)
				-n smaze XML hlavicku";
				die(0);
			}
			else if ($argv[0] != $arg)
			{
				fwrite(STDERR, "Spatne parametry\n");
				die(1);
			}				
		}
		/* Neni zadana query */
		if (!isset($this->query) && !isset($this->queryfile))
		{
			fwrite(STDERR, "Chybi query\n");
			die(1);
		}
	}
	/* Funkce overujici vstupni XML. */
	public function decodeXML()
	{	
		/* Pokud neni zadan input, nacita se ze STDIN */	
		if (!isset($this->inputfilename))
		{	
			$tmp='';
			$handle = fopen('php://stdin', 'r');
			while(!feof($handle))
			{
			    $buffer = fgets($handle);
			    $tmp = $tmp.$buffer;			    
			}
			fclose($handle);
			$this->inputxml = simplexml_load_file($tmp);						
			if ($this->inputxml == FALSE)
			{
				fwrite(STDERR, "Spatny soubor\n");
				die(2);
			}
		}
		else
		{
			$this->inputxml = simplexml_load_file($this->inputfilename);
			if ($this->inputxml == FALSE)
			{
				fwrite(STDERR, "Spatny soubor\n");
				die(2);
			}
		}
	}
	/* Funkce, ktera zpracovana argument --query */
	public function decodeQuery($outputXML)
	{
		if (isset($this->query))
		{
			$this->query = str_replace('=', ' = ', $this->query);
			$this->query = str_replace('<', ' < ', $this->query);
			$this->query = str_replace('>', ' > ', $this->query);
			$this->query = preg_replace('/\s+/', ' ', $this->query);
		}
		else if (isset($this->queryfile))
		{
			$this->query = str_replace('=', ' = ', $this->queryfile);
			$this->query = str_replace('<', ' < ', $this->query);
			$this->query = str_replace('>', ' > ', $this->query);			
			$this->query = preg_replace('/\s+/', ' ', $this->query);
		}		
		else die(1);
		$this->query = trim($this->query);
		$q = explode(" ",$this->query);
		$length = count($q);
		$counter = 1;		
		$error = "Chyba v SQL dotazu";
		try
		{
			if ($length < 3)	throw new Exception("Neúplný SQL dotaz.");
			if ($q[0] != "SELECT")	throw new Exception("Očekávaný token: SELECT, realita: ".$q[0]);			
			$outputXML->element = $q[1];
			if ($length > 2)
			{
				/* LIMIT je volitelny */
				if ($q[$counter+1] == "LIMIT")
				{		
					if (is_numeric($q[$counter+2]))
					{
						/* Na zaklade explode podle tecky poznam, zda je zadan integer */
						if (sizeof(explode('.',$q[$counter+2])) != 1) 
							throw new Exception("Literal musi byt cele cislo.");
						$outputXML->limit = (int)$q[$counter+2];	
						$counter = $counter+2;					
					}
					else throw new Exception("Očekáván integer za LIMIT.");
				}
				if ($q[$counter+1] == "FROM")
				{
					if (isset($q[$counter+2]))
					{
						/* Na zaklade explode podle tecky v budoucnu poznam, zda byl zadan element, element.attribute
						nebo .attribute */
						$outputXML->from = explode(".",$q[$counter+2]);
						$counter = $counter+2;
					}	
					else $counter++;				
				}
				else throw new Exception("Neúplný SQL dotaz.");	
				/* Jeste zbyvaji zpracovat dalsi kusy query -> pravdepodobne WHERE */			
				if ($counter + 4 <= $length)
				{
					if ($q[$counter+1] == "WHERE")
					{
						if ($q[$counter+2] == "NOT")
						{
							$outputXML->whereNot = true;
							$counter++;
						}
						/* Na zaklade explode podle tecky v budoucnu poznam, zda byl zadan element, element.attribute
						nebo .attribute */
						$outputXML->whereElem = explode(".",$q[$counter+2]);
						if (sizeof($outputXML->whereElem) > 2) throw new Exception("Špatná sémantika");
						if ($q[$counter+3] == "=" || $q[$counter+3] == ">" || $q[$counter+3] == "<" || $q[$counter+3] == "CONTAINS") $outputXML->whereOper = $q[$counter+3];
						else throw new Exception("Špatný operátor.");
						$outputXML->whereLiteral = $q[$counter+4];
						$counter = $counter + 4;
						/* Slouceni stringu mezi uvozovkama, pokud byl rozdelen mezerou */
						if (strlen(strrchr($outputXML->whereLiteral, '"')) > 1)
						{							
							while(isset($q[$counter+1]))
							{
								if (strlen(strrchr($q[$counter+1], '"')) > 1)								
									throw new Exception("Spatne zadany string");								
								$outputXML->whereLiteral = $outputXML->whereLiteral." ".$q[$counter+1];
								$counter++;
								if (strlen(strrchr($q[$counter], '"')) == 1)
								{
									break;
								}
							}
						}
					}				
				}
				if ($counter+1 != $length) throw new Exception("chybí nebo přebývají některé prvky..");
			}
		}
		catch (Exception $e)
		{
			fwrite(STDERR, "Chyba v SQL dotazu - ".$e->getMessage()."\n");			
			die(80);
		}		
	}
}
/* Samotny program */
$input = new XMLin();
$outputXML = new XMLout();
$input->decodeArguments($argv, $argc);
$input->decodeXML();
$input->decodeQuery($outputXML);
/* Pokud je zadan parametr -n, smaze se XML hlavicka */
if (!isset($input->root))
	$outputXML->output = new SimpleXMLElement("<?xml version=\"1.0\" encoding=\"utf-8\"?><noRootBooy></noRootBooy>");
else
	$outputXML->output = new SimpleXMLElement("<?xml version=\"1.0\" encoding=\"utf-8\"?><".$input->root."></".$input->root.">");
$outputXML->searchXML($outputXML->searchFrom($input->inputxml));
$asString = $outputXML->output->asXML();
if (isset($input->noHeader))
	$asString = preg_replace("/^.+\n/","",$asString);
if (!isset($input->root))
{
	$asString = preg_replace("/<noRootBooy>/","",$asString);
	$asString = preg_replace("/<\/noRootBooy>/","",$asString);
	$asString = preg_replace("/<noRootBooy\/>/","",$asString);
}
$asString=str_replace("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n", "<?xml version=\"1.0\" encoding=\"utf-8\"?>", $asString);
if (isset($input->outputfilename))
	file_put_contents($input->outputfilename, $asString);
/* Pokud neni zadan output, vysledek je zapsan do STDOUT */
else
	fwrite(STDOUT, $asString);
die(0);
?>
