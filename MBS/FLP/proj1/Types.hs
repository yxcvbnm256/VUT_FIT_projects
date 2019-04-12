{-|
simplify-bkg
author: Jan Pawlus (xpawlu00@stud.fit.vutbr.cz)
-}
module Types
    where

-- | Data type representing a symbol. A symbol can be a nonterminal, a terminal or an empty word (epsilon).
data Symbol
	= Nonterminal Char
	| Terminal Char
	| Epsilon
  deriving (Show, Eq)

-- | Data type representing an input, which can be either a file name or a standard input if file name is not specified.
data Input
    = StdInput
    | FileInput String
  deriving (Show)

-- | Data structure representing a rule in an context-free grammar. There is a left-sided nonterminal and right-sided list of symbols as an output.
data Rule = Rule
	{ nonterminal :: Symbol
	, output :: [Symbol] }
  deriving (Show)

-- | Data structure representing a context-free grammar. There is a list of nonterminals, list of terminals, starting nonterminal and rule list.
data BKG = BKG
	{ nonterminals :: [Symbol]
	, terminals :: [Symbol]
	, startNonterminal :: Symbol
	, rules :: [Rule]
	}
  deriving (Show)

-- | Data structure representing program arguments, which can be -i, -1, -2 and specification of an input.
data Options = Options
    { justPrint :: Bool
    , firstPhasePrint :: Bool
    , secondPhanePrint :: Bool
    , input :: Input }
  deriving (Show)