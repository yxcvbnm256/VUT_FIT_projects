{-|
simplify-bkg
author: Jan Pawlus (xpawlu00@stud.fit.vutbr.cz)
-}
module Options (opts) where

import Options.Applicative
import Data.Semigroup ((<>))

import Types

-- | A function that defines the input file argument
fileInput :: Parser Input
fileInput = FileInput <$> argument str (metavar "FILE")

-- | A function that chooses either a standard or file input
inputOption :: Parser Input
inputOption = fileInput <|> pure StdInput

-- | A function that parses the arguments according to the Option.Applicative docs
opts :: Parser Options
opts = Options
	<$> switch
		( short 'i'
		<> help "Just print" )
	<*> switch
		( short '1'
		<> help "Prints after first phase" )
	<*> switch
		( short '2'
		<> help "Prints after second phase" )
	<*> inputOption