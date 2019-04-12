{-|
simplify-bkg
author: Jan Pawlus (xpawlu00@stud.fit.vutbr.cz)
-}
module Output
    where

import Types

-- | A function that returns an characer representation of a symbol.
returnSymbol :: Symbol -> Char
returnSymbol (Nonterminal a) = a
returnSymbol (Terminal a) = a
returnSymbol Epsilon = '#'

-- | A function that creates a string line of symbols from a symbol array and divides each symbol with specified delimiter.
symbolsToString :: [Symbol] -> String -> String
symbolsToString symbols delimiter = concatMap (\x -> 
	((addDelimiter delimiter x symbols)  ++ [returnSymbol x])) symbols
	where
		addDelimiter delimiter symbol symbols
			| (symbols !! 0) == symbol = ""
			| otherwise = delimiter

-- | A function that creates lines of string of rules.
rulesToString :: [Rule] -> String
rulesToString rules = concatMap (\ x -> (ruleToString x) ++ "\n") rules
	where
		ruleToString (Rule nonterminal output) = [returnSymbol nonterminal] ++ "->" ++ (symbolsToString output "")

-- | A function that creates a string from a context-free grammar structure.
bkgToString :: BKG -> String
bkgToString bkg@(BKG nonterminals terminals startTerminal rules) = 
	(symbolsToString nonterminals ",") ++ "\n" 
	++ (symbolsToString terminals ",") ++ "\n" 
	++ [returnSymbol startTerminal] ++ "\n" 
	++ rulesToString rules
