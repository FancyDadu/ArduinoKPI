# ArduinoKPI
----------------------------------------------------------------------------------------------------
14/12/12

L'idea di base è questa:
ogni volta che sarà necessario processare messaggi in XML si lavorerà a chunks (buffers) di caratteri.
I caratteri presenti nei buffer verranno valutati secondo il loro significato in XML : ad esempio il left bracket '<' significa l'inizio di un tag ,ma se il successivo carattere ( scartando whitespaces ) è '/' è un tag di chiusura , e cosi via...si può quindi usare una sorta di macchina a stati per riconoscere varie sequenze.

La funzione implementata receiveAndStore gestisce i messaggi in entrata . Finchè il server ha da scrivere , la funzione bufferizza un tot di caratteri alla volta , da processare secondo 2 modalità : 
-STORE_NO_FORMATTING : scrive i messaggi sull'sd come si presentano da server , senza apporre newlines o analizzare contenuti
-STORE_WITH_FORMATTING : processa carattere per carattere ciò che è dentro il buffer , e pone newlines dove necessario.

Le funzioni base successive saranno : 

composeMessage : chiamata da funzioni wrappers come join(),insert()... il funzionamento pensato è che a seconda del tipo di wrapper , Arduino seleziona un certo template xml privo di contenuti memorizzato nella SD ; inoltre la funzione riceve in entrata una table con nomi di elements da compilare e il valore da apporre. Arduino quindi analizzerà carattere per carattere il template XML in cerca dei tag desiderati ,compilando il messaggio, e ogni volta che trova un tagName contenuto nella table mette il contenuto nel messaggio.

analyzeMessage : analogo alla funzione sopra, i file ora analizzati saranno le copie dei messaggi ricevuti da server risiedenti nella SD Card ; Arduino avrà il compito di riconoscere i tagName di contents richiesti (inseriti fra i parametri) in mezzo al messaggio e quindi restituire una table con i valori dei contents richiesti

! Se c'è la necessità che Arduino analizzi i messaggi da server "on the fly" senza prima memorizzarli nella SD (che dividerebbe tale compito in due step separati -> maggiore lentezza) sarebbe possibile fare una terza funzione receiveAndParse , estremamente simile alle due sopra.


Tutte e tre le funzioni utilizzerebbero processChar() per riconoscere la sintassi all'interno dei vari buffer ; la vera domanda è :
-è accettabile come approccio? O è inutilmente complicato / inaffidabile / etc..
-processare carattere per carattere in questo modo : time consuming? alternative migliori? etc.. (alla fine in ogni caso per trovare tags all'interno di un file/buffer/messaggio/etc bisognerebbe analizzare ogni carattere in cerca di '<' , '>' , '/' etc.. , ma potrebbero esserci metodi migliori di altri / già pronti all'uso / etc...!)

----------------------------------------------------------------------------------------------------
