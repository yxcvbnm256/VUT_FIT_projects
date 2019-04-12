{-|
simplify-bkg
author: Jan Pawlus (xpawlu00@stud.fit.vutbr.cz)
-}
module Main (main) where

import Options.Applicative
import Data.Semigroup ((<>))

import Types
import Options
import Parser
import Output
import Simplify

-- | Main function that behaves according to Options.Applicative docs
main :: IO ()
main = execParser desc >>= getInput
	where
		desc = info (helper <*> opts)
			( fullDesc
				<> progDesc "Program that simplifies a context-free grammar"
				<> header "simplify-bkg" )

-- | A function that gets the input grammar either from standard input or an input file
getInput :: Options -> IO ()
getInput opts@(Options justPrint firstPhasePrint secondPhasePrint StdInput) = do
	grammar <- getContents	
	handleBKG opts $ parseInput grammar
getInput opts@(Options justPrint firstPhasePrint secondPhasePrint (FileInput fname)) = do
	grammar <- readFile fname
	handleBKG opts $ parseInput grammar

-- | A function that executes the application's logic according to the arguments
handleBKG :: Options -> BKG -> IO ()
handleBKG opts@(Options justPrint firstPhasePrint secondPhasePrint _) bkg@(BKG nonterminals terminals startNonterminal rules)
	| justPrint = putStr $ bkgToString bkg
	| firstPhasePrint = putStr $ bkgToString $ createFirstPhaseGrammar bkg
	| secondPhasePrint = putStr $ bkgToString $ createSecondPhaseGrammar $ createFirstPhaseGrammar bkg 
	| otherwise = putStr ""
	