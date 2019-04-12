{-|
simplify-bkg
author: Jan Pawlus (xpawlu00@stud.fit.vutbr.cz)
-}
module Simplify
	where

import Types
import Data.List

-- | A function that makes the first phase of the simplify algorithm.
createFirstPhaseGrammar :: BKG -> BKG
createFirstPhaseGrammar bkg@(BKG oldNonterminals oldTerminals startNonterminal oldRules) = 
	let
		nt = computeNt bkg []
		allowedSymbols = oldTerminals `union` nt `union` [Epsilon]
		newRules = filter (\ (Rule nont output) -> elem nont nt && null (output \\ allowedSymbols)) oldRules
		newNonterminals = nt `union` [startNonterminal]
	in
		BKG {
			nonterminals = newNonterminals,
			terminals = oldTerminals,
			startNonterminal = startNonterminal,
			rules = newRules
		}

-- | A function that computes a set of a nonterminals generating terminals according to algorithm 4.1 from TIN
computeNt :: BKG -> [Symbol] -> [Symbol]
computeNt bkg@(BKG nonterminals terminals _ rules) initList 
	| newList == initList = initList
	| otherwise = computeNt bkg newList
		where
			symbols = (initList `union` terminals) `union` [Epsilon]
			newList = filter (\ x -> symbolInRule x symbols rules) nonterminals

-- | A function that decides whether a given nonterminal is included in at least one grammar's rule that meets the condition of an algorithm 4.1 from TIN
symbolInRule :: Symbol -> [Symbol] -> [Rule] -> Bool
symbolInRule sym nt rules = not (null $ filter (\ (Rule nonterminal output) -> nonterminal == sym && null (output \\ nt)) rules)

-- | A function that makes the second phase of the simplify algorithm.
createSecondPhaseGrammar :: BKG -> BKG
createSecondPhaseGrammar bkg@(BKG oldNonterminals oldTerminals startNonterminal oldRules) = 
	let
		v = computeV bkg [startNonterminal]
		newTerminals = oldTerminals `intersect` v
		newNonterminals = oldNonterminals `intersect` v
		newRules = filter (\ (Rule nonterminal output) -> elem nonterminal newNonterminals && null (output \\ (v `union` [Epsilon]))) oldRules
	in
		BKG {
		nonterminals = newNonterminals,
		terminals = newTerminals,
		startNonterminal = startNonterminal,
		rules = newRules
	}

-- | A function that computes a set of available symbols according to algorithm 4.2 from TIN
computeV :: BKG -> [Symbol] -> [Symbol]
computeV bkg@(BKG nonterminals terminals _ rules) initList
	| newList == initList = initList
	| otherwise = (computeV bkg newList `union` initList)
		where
			newList = nub $ concatMap (\ (Rule nonterminal output) -> output) $ filter (\ (Rule nonterminal output) -> elem nonterminal initList) rules


-- | A function that compares two lists of symbols.
--compareSymbols :: (Eq a) => [a] -> [a] -> Bool
--compareSymbols x y = null (x \\ y) && null (y \\ x)