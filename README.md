# sofamyroom

# RELEASE
* 1.0
    * dovrà rilasciare la toolbox come la versione originale: MEX per Linux e Windows e VAST
    * dichiarare versione MATLAB per MEX
* 1.1
    * compilati standalone

## TODO - RELEASE
* ROBARU: dataset generation
  * testare VAST
  * preparare codice di esempio
  * dovrebbe funzionare partendo da load_parameters.m
* DANIELE: buildsystem
  * barbatrucchi per farlo girare su linux: compilare libmysofa, generare deb e installarlo; rimuovere 	tutti i stricmp da mexmain.c
  * Aggiornare libmysofa alla nuova versione
  * da buildsystem compilare la cosa rispetto al sistema operativo
  * Compilare Rommsim per rendere disponibili gli eseguibili in versione Release/x64 per tutte le 	piattaforme
  * Testare la compilazione della MEX su Windows e Linux
* DANIELE: pulire repository
  * Sistemare le cartelle del repo per capire cosa pubblicare e aggiornare di conseguenza il README
  * Dare una ricontrollata al codice di	Roomsim
  * EUPL: aggiornare copyleft su header files	Roomsim
* ROBARU: documentazione
  * sfoltire documentazione
  * preparare istruzioni per compilare le mex (per Windows c'è già il README)
