10/12

Odstranění redundantních symbolů z bezkontextových gramatik.

# simplify-bkg
# author: Jan Pawlus (xpawlu00@stud.fit.vutbr.cz)

## Argument parsing - optparse-applicative (https://github.com/pcapriotti/optparse-applicative) was used for argument parsing.
## Merlin differences - missing Data.Semigroup

## Algorithm description
	First we load either file input or standard input. Then a parser function is called to create a context-less grammar from the input. Next, according to the user's parameters, the program either prints the grammar to standard output or applies first (or second) phase of the simplifying algorithm specified in TIN study materials. If more parameters are set, only one of them will actually take place.

## Modules
	* Main.hs - IO operations here and the application logic controlled here
	* Types.hs - definitions for user types needed for this program are defined here
	* Options.hs - argument parsing via optparse-applicative
	* Parser.hs - creates context-less grammar from the text input
	* Output.hs - creates a text output from a context-less grammar
	* Simplify.hs - both simplifying phases of the algorithm are here

## Error handling
	Most of the error handling is left to the default behaviour or optparse-applicative default behaviour.
