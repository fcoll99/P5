PAV - P5: síntesis musical polifónica
=====================================

Obtenga su copia del repositorio de la práctica accediendo a [Práctica 5](https://github.com/albino-pav/P5) 
y pulsando sobre el botón `Fork` situado en la esquina superior derecha. A continuación, siga las
instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para crear una rama con el apellido de
los integrantes del grupo de prácticas, dar de alta al resto de integrantes como colaboradores del proyecto
y crear la copias locales del repositorio.

Como entrega deberá realizar un *pull request* con el contenido de su copia del repositorio. Recuerde que
los ficheros entregados deberán estar en condiciones de ser ejecutados con sólo ejecutar:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.sh
  make release
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A modo de memoria de la práctica, complete, en este mismo documento y usando el formato *markdown*, los
ejercicios indicados.

Ejercicios.
-----------

### Envolvente ADSR.

Tomando como modelo un instrumento sencillo (puede usar el InstrumentDumb), genere cuatro instrumentos que
permitan visualizar el funcionamiento de la curva ADSR.

* Un instrumento con una envolvente ADSR genérica, para el que se aprecie con claridad cada uno de sus
  parámetros: ataque (A), caída (D), mantenimiento (S) y liberación (R).
  ![Instrument1](https://user-images.githubusercontent.com/61736138/80309832-9bcf6480-87d7-11ea-8ee3-9840763c71c6.PNG)
  
* Un instrumento *percusivo*, como una guitarra o un piano, en el que el sonido tenga un ataque rápido, no
  haya mantenimiemto y el sonido se apague lentamente.
  - Para un instrumento de este tipo, tenemos dos situaciones posibles:
    * El intérprete mantiene la nota *pulsada* hasta su completa extinción.
    	![Instrument2](https://user-images.githubusercontent.com/61736138/80309843-af7acb00-87d7-11ea-839a-16720c9801a0.PNG)
    * El intérprete da por finalizada la nota antes de su completa extinción, iniciándose una disminución
	  abrupta del sonido hasta su finalización.
	  ![Instrument3](https://user-images.githubusercontent.com/61736138/80309844-b0abf800-87d7-11ea-94c8-c047c7fe0b64.PNG)
  - Debera representar en esta memoria **ambos** posibles finales de la nota.
* Un instrumento *plano*, como los de cuerdas frotadas (violines y semejantes) o algunos de viento. En
  ellos, el ataque es relativamente rápido hasta alcanzar el nivel de mantenimiento (sin sobrecarga), y la
  liberación también es bastante rápida.
![Instrument4](https://user-images.githubusercontent.com/61736138/80309846-b275bb80-87d7-11ea-837c-4a8f0872e0af.PNG)


### Instrumentos Dumb y Seno.

Implemente el instrumento `Seno` tomando como modelo el `InstrumentDumb`. La señal **deberá** formarse
mediante búsqueda de los valores en una tabla.

- Incluya, a continuación, el código del fichero `seno.cpp` con los métodos de la clase Seno.
```cpp
#include <iostream>
#include <math.h>
#include <errno.h>
#include <string.h>
#include "seno.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

Seno::Seno(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h    
  */
  KeyValue kv(param);
  std::string file_name = "table.wav";
  static string kv_null;
  unsigned int fm;
  if (readwav_mono(file_name, fm, tbl) < 0) {
    cerr << "Error: no se puede leer el fichero " << file_name << " para un instrumento FicTabla" << " (" << strerror(errno) << ")"<< endl;
    throw -1;
  }
  N = tbl.size();
  phs = 0;
  index = 0;
  stepA = 2 * M_PI /(float) N;
}


void Seno::command(long cmd, long note, long vel) {
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    f0 = 440.0 * pow(2, (((float)note - 69.0)/12.0));
    index=0;
    phs=0;
    stepB = 2 * M_PI * f0/SamplingRate;
	  A = vel / 127.;
    phs=0;
  }
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}


const vector<float> & Seno::synthesize() {
  if (not adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive) return x;
  for (unsigned int i=0; i<x.size(); ++i) {
    unsigned int i1 = (int)ceil(phs);
    unsigned int i2 = (int)floor(phs);
    float a1 = (float) i2 - phs;
    float a2 = (float) phs - i1;
    
    while(i2 >= N) i2 -= N;
    while(i1 >= N) i1 -= N;
    x[i] = A*(tbl[i1]*(a1) + tbl[i2]*(a2));
    phs += stepB/stepA;
    index = phs;
    while(index >= N){
      phs = stepB/stepA;
      index -= N;
    }
  }
  adsr(x); //apply envelope to x and update internal status of ADSR
  return x;
}
```
- Explique qué método se ha seguido para asignar un valor a la señal a partir de los contenidos en la tabla,
  e incluya una gráfica en la que se vean claramente (use pelotitas en lugar de líneas) los valores de la
  tabla y los de la señal generada.
  
  A partir de los valores de la tabla, se ha hecho una interpolación lineal de estos valores para obtener la forma de onda resultante en cada nota.
  ##### Valores de la tabla
  ![tablePeriod](https://user-images.githubusercontent.com/61736138/80309942-5d867500-87d8-11ea-915d-c3f5f15c75e8.PNG)
  ##### Valores de la señal generada a partir de la tabla
  ![doremiSignal](https://user-images.githubusercontent.com/61736138/80309945-5eb7a200-87d8-11ea-9f1e-cd0a3cbb963f.PNG)

- Si ha implementado la síntesis por tabla almacenada en fichero externo, incluya a continuación el código
  del método `command()`.
  
  Los cambios realizados en el método command se pueden observar en el contenido de **seno.cpp**. En él se puede observar que en este método solo se han realizado cambios para calcular la interpolación lineal y evitar evitar saltos en amplitud y fase.

### Efectos sonoros.

- Incluya dos gráficas en las que se vean, claramente, el efecto del trémolo y el vibrato sobre una señal
  sinusoidal. Deberá explicar detalladamente cómo se manifiestan los parámetros del efecto (frecuencia e
  índice de modulación) en la señal generada (se valorará que la explicación esté contenida en las propias
  gráficas, sin necesidad de *literatura*).
  
  ##### Tremolo
  ![tremolo](https://user-images.githubusercontent.com/61736138/80310033-de457100-87d8-11ea-98a3-44157532def9.PNG)

  Cuando se aplica un tremolo, la amplitud de la señal varia creando una envolvente senoidal. En los parámetros de entrada se dedice la frecuencia de oscilación (respecto de la frecuencia de muestreo) y la amplitud de la señal de la envolvente.
  El archivo **doremiTremolo.wav** del directorio `work/ejemplos` demuestra su funcionamiento. 
  
  ##### Vibrato
  ![vibrato](https://user-images.githubusercontent.com/61736138/80310030-dd144400-87d8-11ea-8ca8-2125dc573696.PNG)
  
  Aunque no es tan facil percibir el efecto a simple vista, cuando se aplica un vibrato la frecuencia de la señal varia. Ésta acelera y frena ligeramente alrededor de la frecuencia original de la señal. En los parámetros se decide la frecuencia del efecto y su variación en semitonos.
  El archivo **doremiVibrato.wav** del directorio `work/ejemplos` demuestra su funcionamiento. 
- Si ha generado algún efecto por su cuenta, explique en qué consiste, cómo lo ha implementado y qué
  resultado ha producido. Incluya, en el directorio `work/ejemplos`, los ficheros necesarios para apreciar
  el efecto, e indique, a continuación, la orden necesaria para generar los ficheros de audio usando el
  programa `synth`.
  
  ##### Distorsión
  ![distortion](https://user-images.githubusercontent.com/61736138/80310031-de457100-87d8-11ea-9133-761f7484f6a2.PNG)
  
  Cuando se aplica la distorsión, se modifica la amplitud de la señal a partir de un umbral. Los valores superiores a ese umbral toman el valor de umbral. Si los valores son inferiores al valor negativo del umbral, entonces toman su valor negativo. Para los valores intermedios se ha decidido aplicar un soft clipping, que permite suavizar los extremos.
  
  El código utilizado para implementar este efecto es el siguiente:
  ```cpp
  #include <iostream>
  #include <math.h>
  #include "distortion.h"
  #include "keyvalue.h"

  #include <stdlib.h>

  using namespace upc;
  using namespace std;

  Distortion::Distortion(const std::string &param) {

    KeyValue kv(param);

    if (!kv.to_float("A", A))
      A = 0.5; //default value
  }

  void Distortion::command(unsigned int comm) {
    if (comm == 1);
  }

  void Distortion::operator()(std::vector<float> &x){
      for (unsigned int i = 0; i < x.size(); i++) {
      if (x[i] > A) x[i] = A;
      else if (x[i] < -A) x[i] = -A;
      else x[i] = (x[i] - pow(x[i],3)/3.0)*(3.0/2.0)/A;
    }
  }
  ```

  El archivo **doremiDistortion.wav** del directorio `work/ejemplos` demuestra su funcionamiento.
  Este archivo se puede generar utiltizando los siguientes comandos teniendo en cuenta que partimos del directorio P5:
  ```
  synth work/doremi_seno.orc work/ejemplos/doremiDistortion.sco work/ejemplos/doremiDistortion.wav -e work/effects.orc

  ```

### Síntesis FM.

Construya un instrumento de síntesis FM, según las explicaciones contenidas en el enunciado y el artículo
de [John M. Chowning](https://ccrma.stanford.edu/sites/default/files/user/jc/fm_synthesispaper-2.pdf). El
instrumento usará como parámetros **básicos** los números `N1` y `N2`, y el índice de modulación `I`, que
deberá venir expresado en semitonos.

- Use el instrumento para generar un vibrato de *parámetros razonables* e incluya una gráfica en la que se
  vea, claramente, la correspondencia entre los valores `N1`, `N2` e `I` con la señal obtenida.
  
  Sabemos que los parámetros N1 y N2 nos indican la relación entre la frecuencia carrier (fc) i la moduladora tal que fc=fm·N1/N2; así pues, como mayor sea el cociente, más diferencia habrá entre la frecuencia del tono y entre el vibrato. Por otro lado, la profundidad del vibrato, I, nos indica el nivel máximo y mínimo de variación de la frecuencia expresado en semitonos.
  Para poder observar más claramente estos parámetros, a continucación se muestran dos gráficas:
  
	<img width="379" alt="Fmparam1" src="https://user-images.githubusercontent.com/61736138/80311807-1dc48b00-87e2-11ea-8bdd-32f84d55bc46.png">

  La primera con N1=30 i N2=2, habiendo generado un señal de frecuencia 300Hz; por lo tanto, el valor de la frecuencia portadora es de 20Hz. Por otro lado, al tener I=1, la variación de la frecuencia es mínima.
  
  
	<img width="374" alt="Fmparam2" src="https://user-images.githubusercontent.com/61736138/80311829-45b3ee80-87e2-11ea-8e5d-9ead37187a29.png">
  
  En esta segunda imagen, hemos mantenido los parámetros N1 i N2, pero hemos incrementado la I hasta 10, de tal manera que se puede observar una variación mucho más notoria.

- Use el instrumento para generar un sonido tipo clarinete y otro tipo campana. Tome los parámetros del
  sonido (N1, N2 e I) y de la envolvente ADSR del citado artículo. Con estos sonidos, genere sendas escalas
  diatónicas (fichero `doremi.sco`) y ponga el resultado en los ficheros `work/doremi/clarinete.wav` y
  `work/doremi/campana.work`.
  
  A continuación se muestran los archivos .orc tanto de la campana como del clarinete. Se puede encontrar el resultado .wav en la carpeta `work/doremi/`
  ```
  CAMPANA.ORC
  #Campana
  1   InstrumentChowning  ADSR_A=0.01; ADSR_D=0.7; ADSR_S=0; ADSR_R=1; I=6; N2=3;

  CLARINETE.ORC
  #Clarinet
  1   InstrumentChowning  ADSR_A=0.1; ADSR_D=0; ADSR_S=0.7; ADSR_R=0.1; I=1; N2=3;

  ```

  * También puede colgar en el directorio work/doremi otras escalas usando sonidos *interesantes*. Por
    ejemplo, violines, pianos, percusiones, espadas láser de la
	[Guerra de las Galaxias](https://www.starwars.com/), etc.
	
	A su vez hemos decidido crear otro instrumento como es el piano, ya que lo vamos a necesitar en la implementación de lasdos siguientes canciones. A continuación se muestra sus archivo .orc i .sco ya que hemos modificado las notas para que sean más correspondientes a un piano:
	
  ```
  PIANO.ORC
  #Piano
  1   InstrumentChowning  ADSR_A=0.001; ADSR_D=0.6; ADSR_S=0.0; ADSR_R=1.5; I=1.2; N2=3;

  PIANO.SCO
  0	9	5	50	90
  120	8	5	50	90
  20	9	5	52	90
  120	8	5	52	90
  20	9	5	54	90
  120	8	5	54	90
  20	9	5	55	90
  120	8	5	55	90
  20	9	1	57	90
  120	8	1	57	90
  20	9	1	59	90
  120	8	1	59	90
  20	9	1	61	90
  120	8	1	61	90
  20	9	1	62	90
  120	8	1   62	90
  20	9	1	64	90
  120	8	1	64	90
  20	9	1	65	90
  120	8	1	65	90
  20	9	1	67	90
  120	8	1	67	90
  20	9	1	69	90
  120	8	1	69	90
  20	9	1	71	90
  120	8	1	71	90
  20	9	1	72	90
  120	8	1	72	90
  0	0	1	0	0

  ```


### Orquestación usando el programa synth.

Use el programa `synth` para generar canciones a partir de su partitura MIDI. Como mínimo, deberá incluir la
*orquestación* de la canción *You've got a friend in me* (fichero `ToyStory_A_Friend_in_me.sco`) del genial
[Randy Newman](https://open.spotify.com/artist/3HQyFCFFfJO3KKBlUfZsyW/about).

- En este triste arreglo, la pista 1 corresponde al instrumento solista (puede ser un piano, flautas,
  violines, etc.), y la 2 al bajo (bajo eléctrico, contrabajo, tuba, etc.).
  
  Hemos decidido generar para la pista 1 el piano mencionado anteriormente y, para la pista dos, un bajo.
  
    ```
  ORC_TORYSTORY.ORC
  #Piano
  1   InstrumentChowning  ADSR_A=0.01; ADSR_D=0.7; ADSR_S=0.0; ADSR_R=1.5; I=1; N2=3;
  #Baix
  2   InstrumentChowning  ADSR_A=0.01; ADSR_D=0.0; ADSR_S=0.3; ADSR_R=0.4; I=1.3; N2= 3;
  
  ```
  
- Coloque el resultado, junto con los ficheros necesarios para generarlo, en el directorio `work/music`.

El resultado final consiste en un .wav con el nombre de **ToyStory.wav**
 
- Indique, a continuación, la orden necesaria para generar la señal (suponiendo que todos los archivos
  necesarios están en directorio indicado).
  
  Suponiendo que nos encontramos dentro del directorio `work/music`, la orden para generar la sñal es:
  
  ```
  synth orc_ToyStory.orc ToyStory_A_Friend_in_me.sco ToyStory.wav
  ```
 

También puede orquestar otros temas más complejos, como la banda sonora de *Hawaii5-0* o el villacinco de
John Lennon *Happy Xmas (War Is Over)* (fichero `The_Christmas_Song_Lennon.sco`), o cualquier otra canción
de su agrado o composición. Se valorará la riqueza instrumental, su modelado y el resultado final.

Hemos decidido sintetizar la canción de Hawaii-5.0; pero durante el rpcoeso, nos dimos cuenta que no eramos capazes de sintetizar a la perfección los instrumentos que venian especificados en el .sco. Por lo tanto, nuestra versión esta hecha con el mismo numero de instrumentos, pero de solamente 3 tipos: pianos, bajos y clarinetes. Hemos modificado los parámetros de sus envolventes ADSR para que, por ejemplo, no todos los pianos sean iguales y tengan pequeñas diferencias.

A su vez, hemos sido incapazes de sintetizar el roll i kick que se indicn en el inicio de la canción; así pues, esta, empieza directamente con el iano y bajo tocando la melodia. Por otro lado, había efectos como el hat, snare... que no hemos sabido parametrizarlos únicamente con su envolvente adsr y ls parámetros I, N1 y N2. Por lo tanto, hemos optado por incorporarlos mediante audios ya construidos. Estos se encuentran en la carpeta `work/track` y,para poder incorporarlos, hemos creado una nueva clase llamada `Audio` como si de un instrumento se tratase y, por lo tanto, sigue la misma estructura de: constructor, command y synthetise:

```cpp
#include <iostream>
#include <math.h>
#include "audio.h"
#include "keyvalue.h"
#include "sndfile.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

Audio::Audio(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  KeyValue kv(param);
  
  file_name = kv("file");
  inputFile = sf_open(file_name.c_str(), SFM_READ, &inputFileData);
}


void Audio::command(long cmd, long note, long vel) {
    bActive = true;
    A = vel/127.0;
}


const vector<float> & Audio::synthesize() {

static double *content = new double[BSIZE];

  reader = sf_read_double(inputFile, content, BSIZE);
  if(reader > 0){
    for (unsigned int i = 0; i < reader; i++) x[i] = 0.5*A*content[i];
  }else{
    x.assign(x.size(), 0);
    bActive = false;
    sf_close(inputFile);
  }
  
  return x;
}  
  ```
  Y a continuación se muestra su respectivo fichero .orc:
  
  ```
3 Audio file=tracks/track3.wav
4 Audio file=tracks/track4.wav
5 Audio file=tracks/track5.wav
6 Audio file=tracks/track6.wav
7 Audio file=tracks/track7.wav
#Baix
8   InstrumentChowning  ADSR_A=0.01; ADSR_D=0.0; ADSR_S=0.3; ADSR_R=0.4; I=4; N2= 3;
#Pianos
9   InstrumentChowning  ADSR_A=0.01; ADSR_D=0.7; ADSR_S=0.0; ADSR_R=1.5; I=1; N2=3;
#Baix
10  InstrumentChowning  ADSR_A=0.01; ADSR_D=0.0; ADSR_S=0.3; ADSR_R=0.4; I=2; N2= 3;
11  InstrumentChowning	ADSR_A=0.1; ADSR_D=0.05; ADSR_S=0.2; ADSR_R=0.2; I=5; N2= 3;
#Altres
12  InstrumentChowning  ADSR_A=0.2; ADSR_D=0.02; ADSR_S=0.3; ADSR_R=0.2; I=1; N2 =1.5;
#Piano
13  InstrumentChowning  ADSR_A=0.03; ADSR_D=0.5; ADSR_S=0.1; ADSR_R=0.15; I=2; N2 = 2.5;
14  InstrumentChowning  ADSR_A=0.03; ADSR_D=0.5; ADSR_S=0.1; ADSR_R=0.15; I=1.3; N2 = 2.5;
15  InstrumentChowning  ADSR_A=0.03; ADSR_D=0.5; ADSR_S=0.1; ADSR_R=0.15; I=1.3; N2 = 2.5;
#Baixos
16  InstrumentChowning  ADSR_A=0.03; ADSR_D=0.0; ADSR_S=0.0; ADSR_R=0.5; I=1; N2= 1.5;
  ```
- Coloque los ficheros generados, junto a sus ficheros `score`, `instruments` y `efffects`, en el directorio
  `work/music`.
  El resultado final .wav se puede encontrar en `work/music` , junto a su fichero .orc y .sco.
  
- Indique, a continuación, la orden necesaria para generar cada una de las señales usando los distintos
  ficheros.
  
  Considerando que estamos situdos en el directorio `work/music`, la orden necesaria general la señal es:
  
  `synth Hawaii5-0.orc Hawaii5-0.sco Hawaii_Synth.wav`  
