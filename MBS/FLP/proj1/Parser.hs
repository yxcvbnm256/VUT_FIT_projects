{-|
simplify-bkg
author: Jan Pawlus (xpawlu00@stud.fit.vutbr.cz)
-}
module Parser
	where

import Types
import Data.List.Split

-- | A function that reads the input string by lines and creates a context-free grammar.
parseInput :: String -> BKG
parseInput inStr =
	let
		content = lines inStr
		nts = head content
		ts = head $ tail content
		start = head . tail . tail $ content
		rules = tail . tail . tail $ content
	in
		BKG {
			nonterminals = parseNTS $ nts,
			terminals = parseTS $ ts,
			startNonterminal = Nonterminal (head start),
			rules = parseRules $ rules
		}

-- | A function that parses a line of nonterminals by splitting characters by a specified delimiter.
parseNTS :: String -> [Symbol]
parseNTS inStr = map (\x -> Nonterminal $ head x) (splitOn "," inStr) 

-- | A function that parses a line of terminals by splitting characters by a specified delimiter.
parseTS :: String -> [Symbol]
parseTS inStr = map (\x -> Terminal $ head x) (splitOn "," inStr)

-- | A function that parses lines with rules and creates a rule list
parseRules :: [String] -> [Rule]
parseRules inStr = map (\x -> makeRule $ splitOn "->" x) inStr

-- | A function that parses a splitted line by a -> delimiter to a rule 
makeRule :: [String] -> Rule
makeRule ruleArray = 
	let 
		outputArray = map (\x -> symbolDecide x) (last ruleArray)
	in
		Rule {
			nonterminal = Nonterminal $ head . head $ ruleArray,
			output = outputArray
		}

-- | A function that creates a symbol from a character according to terminal/nonterminal/epsilon representation
symbolDecide :: Char -> Symbol
symbolDecide sym
	| elem sym ['a'..'z'] = Terminal $ sym
	| elem sym ['A'..'Z'] = Nonterminal $ sym
	| otherwise = Epsilon
