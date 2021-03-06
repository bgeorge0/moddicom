#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct _c_data{
    // data passed as parameters
    int xNVoxel,yNVoxel,zNVoxel;
    double xDim,yDim,zDim;
    int newXNVoxel,newYNVoxel,newZNVoxel;
    double newXDim,newYDim,newZDim;
    // Voxel number for each vertex of the recognized Cube
    int xInf,xSup,yInf,ySup,zInf,zSup;
};


void nOrderMoment(double *matrix, int *Nx, int *Ny, int *Nz,double *order,double *mX,double *mY,double *mZ ) {

	mX=0;	mY=0;	mZ=0;
	int x,y,z;
	int numberOfPoints=0;

	for(z=0; z<(*Nz); z++) {
		for(y=0; z<(*Ny); y++) {
			for(x=0; x<(*Nz); x++) {		
				if ( matrix[x + (y * (*Nx)) +  z * ( x + y )] !=0)   {
					*mX += pow(x, *order );
					*mY += pow(y, *order );
					*mZ += pow(z, *order );
					numberOfPoints++;
				}
			}
		}
	}
	*mX = *mX/numberOfPoints;
	*mY = *mY/numberOfPoints;
	*mZ = *mZ/numberOfPoints;
}
//
// _c_TrilinearInterpolation
//
// Effettua l'interpolazione considerando i valori ai vertici e le dimensioni fisiche del cubo
//
double _c_TrilinearInterpolation(double x0y0z0, double x0y0z1, double x0y1z0, double x0y1z1, double x1y0z0, double x1y0z1, double x1y1z0, double x1y1z1, double x0,double y0, double z0, double dx1x0, double dy1y0, double dz1z0, double x, double y,double z) {
  double xd,yd,zd,c00,c01,c10,c11,c0,c1,c;
  xd = (x-x0)/dx1x0;
  yd = (y-y0)/dy1y0;
  zd = (z-z0)/dz1z0;
  c00 = x0y0z0*(1-xd)+x1y0z0*xd;
  c10 = x0y1z0*(1-xd)+x1y1z0*xd;
  c01 = x0y0z1*(1-xd)+x1y0z1*xd;
  c11 = x0y1z1*(1-xd)+x1y1z1*xd;
  c0 = c00*(1-yd)+c10*yd;
  c1 = c01*(1-yd)+c11*yd;
  c = c0*(1-zd)+c1*zd;
  return c;
}

//
// _c_getCubeVertex
//
// Prende le coordinate in floating point del punto da mappare e restituisce l'intero
// corrispondente ai centroidi relativi alle coordinate dei vertici del cubo da interpolare
// La formula per calcolare il Kinf. è   Kinf = int( (2*xPos - dx) / (2 * dx) )
//
void _c_getCubeVertex(struct _c_data * punt, double xPos, double yPos, double zPos, int ct) {

    punt->xInf = (int)((2 * xPos - punt->xDim) / ( 2 * punt->xDim));
    punt->yInf = (int)((2 * yPos - punt->yDim) / ( 2 * punt->yDim));
    punt->zInf = (int)((2 * zPos - punt->zDim) / ( 2 * punt->zDim));
    punt->xSup = punt->xInf + 1;
    punt->ySup = punt->yInf + 1;
    punt->zSup = punt->zInf + 1;

    /*
    if(xPos>=3.4179 && yPos>=-213.1 && zPos<=-813.1) {
      printf("\n*1********************************************");
      printf("\n xpos,ypos,zpos=%lf,%lf,%lf    ",xPos,yPos,zPos);
      printf("\nxDim,yDim,zDim=%lf,%lf,%lf    ",punt->xDim, punt->yDim, punt->zDim);
      printf("\nxNVoxel,yNVoxel,zNVoxel=%d,%d,%d    ",punt->xNVoxel, punt->yNVoxel, punt->zNVoxel);
      printf("\npunt->xInf,punt->yInf,punt->zInf=%d,%d,%d    ",punt->xInf, punt->yInf, punt->zInf);
      printf("\n*********************************************");
    }*/
    
    // se xPos < punt->xDim significa che sono nel bordo. Considera allora il valore del primo valore di X per x0 e x1
    // (in sostanza proietta all'esterno il valore dei pixel). Analogo se maggiore.
    // STO ANDANDO IN OVERRIDE RISPETTO A QUANDO EVENTUALMENTE CALCOLATO PRIMA
    if( (2*xPos-punt->xDim) < punt->xDim/2 ) {punt->xInf = 0; punt->xSup = 0;}
    if( (2*yPos-punt->yDim) < punt->yDim/2 ) {punt->yInf = 0; punt->ySup = 0;}
    if( (2*zPos-punt->zDim) < punt->zDim/2  ) {punt->zInf = 0; punt->zSup = 0;}

    /*
    if(xPos>=3.4179 && yPos>=-213.1 && zPos<=-813.1) {
      printf("\n*2********************************************");
      printf("\n xpos,ypos,zpos=%lf,%lf,%lf    ",xPos,yPos,zPos);
      printf("\nxDim,yDim,zDim=%lf,%lf,%lf    ",punt->xDim, punt->yDim, punt->zDim);
      printf("\nxNVoxel,yNVoxel,zNVoxel=%d,%d,%d    ",punt->xNVoxel, punt->yNVoxel, punt->zNVoxel);
      printf("\npunt->xInf,punt->yInf,punt->zInf=%d,%d,%d    ",punt->xInf, punt->yInf, punt->zInf);
      printf("\n*********************************************");
    }*/

    // Faccio il reciproco di xPos per vedere se non sto' toccando l'altro estremo
    xPos = punt->xDim * punt->xNVoxel - xPos;
    yPos = punt->yDim * punt->yNVoxel - yPos;
    zPos = punt->zDim * punt->zNVoxel - zPos;
    if( (2*xPos-punt->xDim) < punt->xDim/2 ) {punt->xInf = punt->xNVoxel - 1; punt->xSup = punt->xNVoxel - 1;}
    if( (2*yPos-punt->yDim) < punt->yDim/2 ) {punt->yInf = punt->yNVoxel - 1; punt->ySup = punt->yNVoxel - 1;}
    if( (2*zPos-punt->zDim) < punt->zDim/2  ) {punt->zInf = punt->zNVoxel - 1; punt->zSup = punt->zNVoxel - 1;}
    
    /*
    if(xPos>=3.4179 && yPos>=-213.1 && zPos<=-813.1) {
      printf("\n*3********************************************");
      printf("\n xpos,ypos,zpos=%lf,%lf,%lf    ",xPos,yPos,zPos);
      printf("\nxDim,yDim,zDim=%lf,%lf,%lf    ",punt->xDim, punt->yDim, punt->zDim);
      printf("\nxNVoxel,yNVoxel,zNVoxel=%d,%d,%d    ",punt->xNVoxel, punt->yNVoxel, punt->zNVoxel);
      printf("\npunt->xInf,punt->yInf,punt->zInf=%d,%d,%d    ",punt->xInf, punt->yInf, punt->zInf);
      printf("\n*********************************************");
    }*/

}

/*
//
// trilinearInterpolator
//
// Interpola secondo il nuovo passo (newXDim, newYDim, newZDim) la matrice fornita (*values)
// INPUT:
//      xNVoxel,yNVoxel,zNVoxel = numero di voxel della matrice nelle direzioni x,y,z
//      xDim,yDim,zDim = passo lungo gli assi x,y,z
//      newXDim,newYDim,newZDim = NUOVO passo lungo gli assi x,y,z
//      values = matrice di input
//      returnMatrix = matrice di output
void trilinearInterpolator(int *xNVoxel, int *yNVoxel,int *zNVoxel,
                    double *xDim, double *yDim, double *zDim,
                    int *newXNVoxel, int *newYNVoxel, int *newZNVoxel,
                    double *values,double *returnMatrix) {
  double zPos,yPos,xPos;
  double valoreCalcolato;
  struct _c_data *punt;
  int xVoxelProgressivo,yVoxelProgressivo,zVoxelProgressivo;
  int maxNewXVoxel,maxNewYVoxel,maxNewZVoxel;

  // Alloca un puntatore alla struttura _c_data
  punt = (struct _c_data *)calloc(1,sizeof(struct _c_data));
  if( punt == NULL ) return;
  // calcola il nuovo passo e copia i valori in _c_data: l'idea è quella di ridurre il passaggio parametri
  // fra funzioni ed il clone delle variabili per preservare memoria
  punt->newXDim = ((*xDim)*(*xNVoxel))/(*newXNVoxel);
  punt->newYDim = ((*yDim)*(*yNVoxel))/(*newYNVoxel);
  punt->newZDim = ((*zDim)*(*zNVoxel))/(*newZNVoxel);
  punt->xNVoxel = *xNVoxel; punt->yNVoxel = *yNVoxel; punt->zNVoxel = *zNVoxel;
  punt->xDim = *xDim; punt->yDim = *yDim; punt->zDim = *zDim;
  // Pulisci la matrice di destinazione
  for(int ct=0;ct< ((*newXNVoxel)*(*newYNVoxel)*(*newZNVoxel));ct++ ) returnMatrix[ct]=0;

  // Cicla per ogni NUOVO Voxel
  // asse Z
  for( zPos = punt->newZDim/2, zVoxelProgressivo=0; zVoxelProgressivo < *newZNVoxel; zPos+=punt->newZDim, zVoxelProgressivo++ ) {
      // asse Y
      for( yPos = punt->newYDim/2, yVoxelProgressivo=0; yVoxelProgressivo < *newYNVoxel; yPos+=punt->newYDim,yVoxelProgressivo++ ) {
          // asse X
          for( xPos = punt->newXDim/2, xVoxelProgressivo=0; xVoxelProgressivo < *newXNVoxel; xPos+=punt->newXDim, xVoxelProgressivo++ ) {

              // acquisisci i dati relativi ai voxel della vecchia matrice i cui centroidi sono
              // ai vertici del cubo da interpolare
               _c_getCubeVertex( punt , xPos , yPos , zPos, 0 );

              // reperisci i valori di tali voxel dalla vecchia matrice
              // ed effettua l'interpolazione rispetto a tali centroidi
              valoreCalcolato = _c_TrilinearInterpolation(
                      values[punt->zInf*(*xNVoxel)*(*yNVoxel)+punt->yInf*(*xNVoxel)+punt->xInf],  //x0y0z0 (sample value)
                      values[punt->zSup*(*xNVoxel)*(*yNVoxel)+punt->yInf*(*xNVoxel)+punt->xInf],  //x0y0z1 (sample value)
                      values[punt->zInf*(*xNVoxel)*(*yNVoxel)+punt->ySup*(*xNVoxel)+punt->xInf],  //x0y1z0 (sample value)
                      values[punt->zSup*(*xNVoxel)*(*yNVoxel)+punt->ySup*(*xNVoxel)+punt->xInf],  //x0y1z1 (sample value)
                      values[punt->zInf*(*xNVoxel)*(*yNVoxel)+punt->yInf*(*xNVoxel)+punt->xSup],  //x1y0z0 (sample value)
                      values[punt->zSup*(*xNVoxel)*(*yNVoxel)+punt->yInf*(*xNVoxel)+punt->xSup],  //x1y0z1 (sample value)
                      values[punt->zInf*(*xNVoxel)*(*yNVoxel)+punt->ySup*(*xNVoxel)+punt->xSup],  //x1y1z0 (sample value)
                      values[punt->zSup*(*xNVoxel)*(*yNVoxel)+punt->ySup*(*xNVoxel)+punt->xSup],  //x1y1z1 (sample value)
                      punt->xInf*(*xDim)+(*xDim)/2, //x0
                      punt->yInf*(*yDim)+(*yDim)/2, //y0,
                      punt->zInf*(*zDim)+(*zDim)/2, //z0,
                      *xDim, //dx1x0,
                      *yDim, //dy1y0,
                      *zDim, //dz1z0,
                      xPos, yPos, zPos);

              // memorizza il risultato nella nuova struttura
              returnMatrix[ xVoxelProgressivo + yVoxelProgressivo * (*newXNVoxel) + zVoxelProgressivo * ((*newYNVoxel) * (*newXNVoxel))] = valoreCalcolato;
          }
          if(maxNewYVoxel<yVoxelProgressivo) maxNewYVoxel = yVoxelProgressivo;
      }
    if(maxNewZVoxel<zVoxelProgressivo) maxNewZVoxel = zVoxelProgressivo;
  }
  free(punt);
}
*/


void newnewtrilinearInterpolator(
  int *NXold, int *NYold,int *NZold,
  int *NXnew, int *NYnew,int *NZnew,
  double *oldXps, double *oldYps, double *oldZps,
  double *newXps, double *newYps, double *newZps,
  double *values,double *returnMatrix) {
  int ct;
  int zVoxelProgressivo,yVoxelProgressivo,xVoxelProgressivo;
  double zPos,yPos,xPos,valoreCalcolato;
  struct _c_data *punt;
  
  // Alloca un puntatore alla struttura _c_data
  punt = (struct _c_data *)calloc(1,sizeof(struct _c_data));
  if( punt == NULL ) return;
  
  // calcola il nuovo passo e copia i valori in _c_data: l'idea è quella di ridurre il passaggio parametri
  // fra funzioni ed il clone delle variabili per preservare memoria 
  punt->newXDim = *newXps;  punt->newYDim = *newYps;  punt->newZDim = *newZps;
  punt->xNVoxel = *NXold; punt->yNVoxel = *NYold; punt->zNVoxel = *NZold;
  punt->xDim = *oldXps; punt->yDim = *oldYps; punt->zDim = *oldZps;
  int maxNewXVoxel,maxNewYVoxel,maxNewZVoxel;
  
  // Pulisci la matrice di destinazione
  for(int ct=0;ct< ((*NXnew)*(*NYnew)*(*NZnew));ct++ ) returnMatrix[ct]=0;  
  ct=0;
  for( zPos = punt->newZDim/2, zVoxelProgressivo=0; zVoxelProgressivo < *NZnew; zPos+=punt->newZDim, zVoxelProgressivo++ ) {
    for( yPos = punt->newYDim/2, yVoxelProgressivo=0; yVoxelProgressivo < *NYnew; yPos+=punt->newYDim, yVoxelProgressivo++ ) {
      for( xPos = punt->newXDim/2, xVoxelProgressivo=0; xVoxelProgressivo < *NXnew; xPos+=punt->newXDim, xVoxelProgressivo++ ) {
        
        // acquisisci i dati relativi ai voxel della vecchia matrice i cui centroidi sono
        // ai vertici del cubo da interpolare
        _c_getCubeVertex( punt , xPos , yPos , zPos ,ct );
        
        // reperisci i valori di tali voxel dalla vecchia matrice
        // ed effettua l'interpolazione rispetto a tali centroidi
        valoreCalcolato = _c_TrilinearInterpolation(
            values[punt->zInf*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xInf],  //x0y0z0 (sample value)
            values[punt->zSup*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xInf],  //x0y0z1 (sample value)
            values[punt->zInf*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xInf],  //x0y1z0 (sample value)
            values[punt->zSup*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xInf],  //x0y1z1 (sample value)
            values[punt->zInf*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xSup],  //x1y0z0 (sample value)
            values[punt->zSup*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xSup],  //x1y0z1 (sample value)
            values[punt->zInf*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xSup],  //x1y1z0 (sample value)
            values[punt->zSup*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xSup],  //x1y1z1 (sample value)
            punt->xInf*(*oldXps)+(*oldXps)/2, //x0
            punt->yInf*(*oldYps)+(*oldYps)/2, //y0,
            punt->zInf*(*oldZps)+(*oldZps)/2, //z0,
            *oldXps, //dx1x0,
            *oldYps, //dy1y0,
            *oldZps, //dz1z0,
            xPos, yPos, zPos);

        // memorizza il risultato nella nuova struttura
        returnMatrix[ xVoxelProgressivo + yVoxelProgressivo * (*NXnew) + zVoxelProgressivo * ((*NYnew) * (*NXnew))] = valoreCalcolato;        
        // if( valoreCalcolato!=0 ) printf("\nx = %d, y= %d, z= %d",xVoxelProgressivo,yVoxelProgressivo,zVoxelProgressivo);
      }
      if(maxNewYVoxel<yVoxelProgressivo) maxNewYVoxel = yVoxelProgressivo;
  }
    if(maxNewZVoxel<zVoxelProgressivo) maxNewZVoxel = zVoxelProgressivo;
}
  free(punt);
  
  }

void newnewtrilinearInterpolator_onGivenMatrix(
  int *NXold, int *NYold,int *NZold,
  int *NXnew, int *NYnew,int *NZnew,
  double *oldXps, double *oldYps, double *oldZps,
  double *newXps, double *newYps, double *newZps,
  double *values,double *returnMatrix) {
  int ct;
  int zVoxelProgressivo,yVoxelProgressivo,xVoxelProgressivo;
  double zPos,yPos,xPos,valoreCalcolato;
  struct _c_data *punt;
  
  // Alloca un puntatore alla struttura _c_data
  punt = (struct _c_data *)calloc(1,sizeof(struct _c_data));
  if( punt == NULL ) return;
  
  // calcola il nuovo passo e copia i valori in _c_data: l'idea è quella di ridurre il passaggio parametri
  // fra funzioni ed il clone delle variabili per preservare memoria 
  punt->newXDim = *newXps;  punt->newYDim = *newYps;  punt->newZDim = *newZps;
  punt->xNVoxel = *NXold; punt->yNVoxel = *NYold; punt->zNVoxel = *NZold;
  punt->xDim = *oldXps; punt->yDim = *oldYps; punt->zDim = *oldZps;
  int maxNewXVoxel,maxNewYVoxel,maxNewZVoxel;
  
  // Pulisci la matrice di destinazione
  for(int ct=0;ct< ((*NXnew)*(*NYnew)*(*NZnew));ct++ ) returnMatrix[ct]=0;  
  ct=0;
  for( zPos = punt->newZDim/2, zVoxelProgressivo=0; zVoxelProgressivo < *NZnew; zPos+=punt->newZDim, zVoxelProgressivo++ ) {
    for( yPos = punt->newYDim/2, yVoxelProgressivo=0; yVoxelProgressivo < *NYnew; yPos+=punt->newYDim, yVoxelProgressivo++ ) {
      for( xPos = punt->newXDim/2, xVoxelProgressivo=0; xVoxelProgressivo < *NXnew; xPos+=punt->newXDim, xVoxelProgressivo++ ) {
        
        // acquisisci i dati relativi ai voxel della vecchia matrice i cui centroidi sono
        // ai vertici del cubo da interpolare
        _c_getCubeVertex( punt , xPos , yPos , zPos ,ct );
        
        // reperisci i valori di tali voxel dalla vecchia matrice
        // ed effettua l'interpolazione rispetto a tali centroidi
        valoreCalcolato = _c_TrilinearInterpolation(
          values[punt->zInf*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xInf],  //x0y0z0 (sample value)
          values[punt->zSup*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xInf],  //x0y0z1 (sample value)
          values[punt->zInf*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xInf],  //x0y1z0 (sample value)
          values[punt->zSup*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xInf],  //x0y1z1 (sample value)
          values[punt->zInf*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xSup],  //x1y0z0 (sample value)
          values[punt->zSup*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xSup],  //x1y0z1 (sample value)
          values[punt->zInf*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xSup],  //x1y1z0 (sample value)
          values[punt->zSup*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xSup],  //x1y1z1 (sample value)
          punt->xInf*(*oldXps)+(*oldXps)/2, //x0
          punt->yInf*(*oldYps)+(*oldYps)/2, //y0,
          punt->zInf*(*oldZps)+(*oldZps)/2, //z0,
          *oldXps, //dx1x0,
          *oldYps, //dy1y0,
          *oldZps, //dz1z0,
          xPos, yPos, zPos);
        
        // memorizza il risultato nella nuova struttura
        returnMatrix[ xVoxelProgressivo + yVoxelProgressivo * (*NXnew) + zVoxelProgressivo * ((*NYnew) * (*NXnew))] = valoreCalcolato;        
        // if( valoreCalcolato!=0 ) printf("\nx = %d, y= %d, z= %d",xVoxelProgressivo,yVoxelProgressivo,zVoxelProgressivo);
      }
      if(maxNewYVoxel<yVoxelProgressivo) maxNewYVoxel = yVoxelProgressivo;
    }
    if(maxNewZVoxel<zVoxelProgressivo) maxNewZVoxel = zVoxelProgressivo;
  }
  free(punt);
  
}

/*
 * NAME: newnewtrilinearInterpolator_onGivenPoints
 * NXold,NYold,NZold : numero di voxel della matrice in ingresso
 * oldXps,oldYps,oldZps : pixelSpacing lungo le 3 direzioni della matrice in ingresso
 * newXpts,newYpts,newZpts : lista dei punti su cui andare ad interpolare
 * pts_x,pts_y,pts_z : numero di voxel della nuova matrice
 * values : matrice in ingresso (in forma di array)
 * returnMatrix : matrice di uscita (in forma di array)
 */
void newnewtrilinearInterpolator_onGivenPoints(
  int *NXold, int *NYold,int *NZold,
  double *oldXps, double *oldYps, double *oldZps,
  double *newXpts, double *newYpts, double *newZpts, int *pts_x, int *pts_y, int *pts_z,
  double *values,double *returnMatrix) {
  int ct;
  int zVoxelProgressivo,yVoxelProgressivo,xVoxelProgressivo;
  double zPos,yPos,xPos,valoreCalcolato;
  struct _c_data *punt;
  int xIndex,yIndex,zIndex;

  // Alloca un puntatore alla struttura _c_data
  punt = (struct _c_data *)calloc(1,sizeof(struct _c_data));
  if( punt == NULL ) return;
  
  // calcola il nuovo passo e copia i valori in _c_data: l'idea è quella di ridurre il passaggio parametri
  // fra funzioni ed il clone delle variabili per preservare memoria 
  //punt->newXDim = *newXps;  punt->newYDim = *newYps;  punt->newZDim = *newZps;
  punt->xNVoxel = *NXold; punt->yNVoxel = *NYold; punt->zNVoxel = *NZold;
  punt->xDim = *oldXps; punt->yDim = *oldYps; punt->zDim = *oldZps;
  int maxNewXVoxel,maxNewYVoxel,maxNewZVoxel;
  
  // Pulisci la matrice di destinazione
  for(int ct=0;ct< ((*pts_x) * (*pts_y) * (*pts_z)); ct++ ) returnMatrix[ct]=0;  
  ct=0;

//  for(  ct=0; ct<*numberOfPts; ct++ ) {
  for( zIndex = 0; zIndex < *pts_z; zIndex++ )   {
//    printf("\n ===> %d",zIndex);
    for( yIndex = 0; yIndex < *pts_y; yIndex++ )   {
      for( xIndex = 0; xIndex < *pts_x; xIndex++ )   {
        xPos = newXpts[xIndex];
        yPos = newYpts[yIndex];
        zPos = newZpts[zIndex];
        // acquisisci i dati relativi ai voxel della vecchia matrice i cui centroidi sono
        // ai vertici del cubo da interpolare
        _c_getCubeVertex( punt , xPos , yPos , zPos ,ct );

        // reperisci i valori di tali voxel dalla vecchia matrice
        // ed effettua l'interpolazione rispetto a tali centroidi
        valoreCalcolato = _c_TrilinearInterpolation(
          values[punt->zInf*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xInf],  //x0y0z0 (sample value)
          values[punt->zSup*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xInf],  //x0y0z1 (sample value)
          values[punt->zInf*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xInf],  //x0y1z0 (sample value)
          values[punt->zSup*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xInf],  //x0y1z1 (sample value)
          values[punt->zInf*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xSup],  //x1y0z0 (sample value)
          values[punt->zSup*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xSup],  //x1y0z1 (sample value)
          values[punt->zInf*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xSup],  //x1y1z0 (sample value)
          values[punt->zSup*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xSup],  //x1y1z1 (sample value)
          punt->xInf*(*oldXps)+(*oldXps)/2, //x0
          punt->yInf*(*oldYps)+(*oldYps)/2, //y0,
          punt->zInf*(*oldZps)+(*oldZps)/2, //z0,
          *oldXps, //dx1x0,
          *oldYps, //dy1y0,
          *oldZps, //dz1z0,
          xPos, yPos, zPos);
        
        // memorizza il risultato nella nuova struttura
        returnMatrix[ ct ] = valoreCalcolato; 
        ct++;
      }
    }
  }
  free(punt);
}
/*
 * NAME: newnewtrilinearInterpolator_onGivenPoints
 * NXold,NYold,NZold : numero di voxel della matrice in ingresso
 * oldXps,oldYps,oldZps : pixelSpacing lungo le 3 direzioni della matrice in ingresso
 * newXpts,newYpts,newZpts : lista dei punti su cui andare ad interpolare
 * numberOfPts : numero di punti da interpolare
 * pts_x,pts_y,pts_z :
 */
void old_newnewtrilinearInterpolator_onGivenPoints(
    int *NXold, int *NYold,int *NZold,
    double *oldXps, double *oldYps, double *oldZps,
    double *newXpts, double *newYpts, double *newZpts, int *numberOfPts,
    double *values,double *returnMatrix) {
  int ct;
  int zVoxelProgressivo,yVoxelProgressivo,xVoxelProgressivo;
  double zPos,yPos,xPos,valoreCalcolato;
  struct _c_data *punt;
  
  // Alloca un puntatore alla struttura _c_data
  punt = (struct _c_data *)calloc(1,sizeof(struct _c_data));
  if( punt == NULL ) return;
  
  // calcola il nuovo passo e copia i valori in _c_data: l'idea è quella di ridurre il passaggio parametri
  // fra funzioni ed il clone delle variabili per preservare memoria 
  //punt->newXDim = *newXps;  punt->newYDim = *newYps;  punt->newZDim = *newZps;
  punt->xNVoxel = *NXold; punt->yNVoxel = *NYold; punt->zNVoxel = *NZold;
  punt->xDim = *oldXps; punt->yDim = *oldYps; punt->zDim = *oldZps;
  int maxNewXVoxel,maxNewYVoxel,maxNewZVoxel;
  
  
  printf("\n %d,%d,%d - %lf,%lf,%lf - %lf,%lf,%lf,%d",*NXold,*NYold,*NZold,*oldXps,*oldYps,*oldZps,*newXpts,*newYpts,*newZpts,*numberOfPts);
  return;
  
  
  // Pulisci la matrice di destinazione
  for(int ct=0;ct< *numberOfPts; ct++ ) returnMatrix[ct]=0;  
  ct=0;
  
  for(  ct=0; ct<*numberOfPts; ct++ ) {
    xPos = newXpts[ct];
    yPos = newYpts[ct];
    zPos = newZpts[ct];
    // acquisisci i dati relativi ai voxel della vecchia matrice i cui centroidi sono
    // ai vertici del cubo da interpolare
    _c_getCubeVertex( punt , xPos , yPos , zPos ,ct );
    //       printf("\n (%d,%d,%d) X=%lf,%lf  Y=%lf,%lf  Z=%lf,%lf",xPos,yPos,zPos,punt->xInf,punt->xSup,punt->yInf,punt->ySup,punt->zInf,punt->zSup);
    //        return;
    printf("\n %d ",ct);
    if(ct>=10000) return;
    
    // reperisci i valori di tali voxel dalla vecchia matrice
    // ed effettua l'interpolazione rispetto a tali centroidi
    valoreCalcolato = _c_TrilinearInterpolation(
      values[punt->zInf*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xInf],  //x0y0z0 (sample value)
            values[punt->zSup*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xInf],  //x0y0z1 (sample value)
                  values[punt->zInf*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xInf],  //x0y1z0 (sample value)
                        values[punt->zSup*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xInf],  //x0y1z1 (sample value)
                              values[punt->zInf*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xSup],  //x1y0z0 (sample value)
                                    values[punt->zSup*(*NXold)*(*NYold)+punt->yInf*(*NXold)+punt->xSup],  //x1y0z1 (sample value)
                                          values[punt->zInf*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xSup],  //x1y1z0 (sample value)
                                                values[punt->zSup*(*NXold)*(*NYold)+punt->ySup*(*NXold)+punt->xSup],  //x1y1z1 (sample value)
                                                      punt->xInf*(*oldXps)+(*oldXps)/2, //x0
                                                      punt->yInf*(*oldYps)+(*oldYps)/2, //y0,
                                                      punt->zInf*(*oldZps)+(*oldZps)/2, //z0,
                                                      *oldXps, //dx1x0,
                                                      *oldYps, //dy1y0,
                                                      *oldZps, //dz1z0,
                                                      xPos, yPos, zPos);
    
    // memorizza il risultato nella nuova struttura
    returnMatrix[ ct ] = valoreCalcolato;        
  }
  free(punt);
}


