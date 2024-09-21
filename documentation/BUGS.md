+ refreshing can go wrong, characters being left over in what should be open space,
i have no clue how, when or why it happens; seems like something i can live with
+ readline calling `return_input_available()` will SIGILL in debug builds;
as far as im aware, its gcc 13 being retarded on nested functions
