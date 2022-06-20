#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* this program generates the minimum throughput problem for mobile gateways with delivery at the edge (cplex input)*/

char Empty (int *clusters,int n_devices)
{ for (int i=0;i < n_devices; i++)
    if (clusters[i]==1)
       return 0;
  return 1;
}

int main (int argc, char *argv[]) // param: <fileClusters> <fileEdge> <fileDelays> <fileSpectrum> <nClusters> <nGateways> <nDevices> <freshness> <fileOutput>
{ FILE *in_clusters, *in_edge, *in_delays, *in_spectrum, *out_file; 
  int *mat_clusters, *vec_edge, *mat_delays, *mat_spectrum;

  int n_clusters=atoi(argv[5]);           // total number of clusters 
  int n_gateways=atoi(argv[6]);           // total number of gateways
  int n_devices=atoi(argv[7]);            // total number of IoT devices
  int freshness=atoi(argv[8]);            // data freshness
  char str_in[200],str_out[200];          // input and output strings
  char *ptr;
  int cluster;
  
  if (argc <9)
  { printf("Sintaxe error: program <fileClusters> <fileEdge> <fileDelays> <fileSpectrum> <nClusters> <nGateways> <nDevices> <freshness> <fileOutput>\n");
    exit(0);
  }
  if (!(in_clusters=fopen(argv[1],"r")))
  { printf("Input file %s does not exist\n",argv[1]);
    exit(0);
  }
  if (!(in_edge=fopen(argv[2],"r")))
  { printf("Input file %s does not exist\n",argv[2]);
    exit(0);
  } 
  if (!(in_delays=fopen(argv[3],"r")))
  { printf("Input file %s does not exist\n",argv[3]);
    exit(0);
  } 
  if (!(in_spectrum=fopen(argv[4],"r")))
  { printf("Input file %s does not exist\n",argv[4]);
    exit(0);
  } 
  if (!(out_file=fopen(argv[9],"w")))
  { printf("Output file %s error\n",argv[8]);
    exit(0);	      
  }
  
  // fill clusters matrix from input information
  printf("Clusters: \n");
  mat_clusters=(int *) calloc (n_clusters*n_devices,sizeof(int));  
  fgets(str_in,200,in_clusters);
  for (int c=0;c < n_clusters; c++)
  {  fgets(str_in,200,in_clusters);
     ptr=strtok(str_in," ");
     ptr=strtok(NULL," ");
     while (ptr!=NULL)
     { mat_clusters[(c*n_devices)+atoi(ptr)]=1;

       ptr=strtok(NULL," ");
     }
  }
  for (int c=0;c < n_clusters; c++)
  { for (int i=0;i < n_devices; i++)
      printf("%d ",mat_clusters[(c*n_devices)+i]);
    printf("\n");
  }
  // fill edge matrix from input information
  printf("Edge: \n");
  vec_edge=(int *) calloc (n_clusters,sizeof(int));  
  fgets(str_in,200,in_edge);
  fgets(str_in,200,in_edge);
  ptr=strtok(str_in," ");
  while (ptr!=NULL)
  { vec_edge[atoi(ptr)]=1;
    ptr=strtok(NULL," ");
  }
  for (int c=0;c < n_clusters; c++)
    printf("%d ",vec_edge[c]);
  printf("\n");
  
  // fill delays matrix from input information
  printf("Delays: \n");
  mat_delays=(int *) calloc (n_clusters*n_clusters,sizeof(int));  
  fgets(str_in,200,in_delays);
  for (int cj=0;cj < n_clusters; cj++)
    for (int ck=0;ck < n_clusters; ck++)
    {  fgets(str_in,200,in_delays);
       mat_delays[(cj*n_clusters)+ck]=atoi(str_in);
    }
 
  for (int cj=0;cj < n_clusters; cj++)
  { for (int ck=0;ck < n_clusters; ck++)
      printf("%d ",mat_delays[(cj*n_clusters)+ck]);
    printf("\n");
  }

// fill spectrum matrix from input information
  printf("Spectrum: \n");
  mat_spectrum=(int *) calloc (n_clusters*n_devices,sizeof(int));  
  fgets(str_in,200,in_spectrum);
  for (int i=0;i < n_devices; i++)
    for (int c=0;c < n_clusters; c++)
    {  fgets(str_in,200,in_spectrum);
       ptr=strtok(str_in," ");
       ptr=strtok(NULL," ");
       ptr=strtok(NULL," ");
       mat_spectrum[(i*n_clusters)+c]=atoi(ptr);
    }

   for (int i=0;i < n_devices; i++)
   { for (int c=0;c < n_clusters; c++)
       printf("%d ",mat_spectrum[(i*n_clusters)+c]);
     printf("\n");
   }

  //------------------------------
  // Puts objective function file
  //------------------------------
  sprintf(str_out,"minimize\n");
  fputs(str_out,out_file);
  sprintf(str_out,"obj: tauMAX\n");
  fputs(str_out,out_file);
  
  sprintf(str_out,"subject to\n");
  fputs(str_out,out_file);

  //------------------------------
  // Constraint (4)
  //------------------------------
  int constraint=0;
  
  for (int g=0;g < n_gateways; g++)
  { // virtual cluster 0
    sprintf(str_out,"C4_%d: ",constraint++);  
    fputs(str_out,out_file);
    for (int c=1;c < n_clusters; c++)
    { sprintf(str_out,"+ la_%d_%d_%d ",g,0,c);  
      fputs(str_out,out_file);
    }
    fputs("= 1\n",out_file);
    // virtual cluster |C|+1
    sprintf(str_out,"C4_%d: ",constraint++);  
    fputs(str_out,out_file);
    for (int c=0;c < n_clusters-1; c++)
    { sprintf(str_out,"- la_%d_%d_%d ",g,c,n_clusters-1);  
      fputs(str_out,out_file);
    }
    fputs("= -1\n",out_file);
    // non virtual clusters
    for (int cj=1;cj < n_clusters-1; cj++)
    {  sprintf(str_out,"C4_%d: ",constraint++);
       fputs(str_out,out_file);
       for (int ck=0;ck < n_clusters; ck++)
       { sprintf(str_out,"+ la_%d_%d_%d ",g,cj,ck);  
         fputs(str_out,out_file);
       }
       for (int ck=0;ck < n_clusters; ck++)
       { sprintf(str_out,"- la_%d_%d_%d ",g,ck,cj);  
         fputs(str_out,out_file);
       }
       fputs("= 0\n",out_file);
    }
  }

  //------------------------------
  // Constraint (5)
  //------------------------------
  constraint=0;
  for (int g=0;g < n_gateways; g++)
    for (int c=0;c < n_clusters; c++)
    { sprintf(str_out,"C5_%d: tauA_%d_%d - tauT_%d_%d <= 0\n",constraint++,g,c,g,c);
      fputs(str_out,out_file);
    }
  for (int g=0;g < n_gateways; g++)
    for (int c=0;c < n_clusters; c++)
    { sprintf(str_out,"C5_%d: tauT_%d_%d - tauD_%d_%d <= 0\n",constraint++,g,c,g,c);
      fputs(str_out,out_file);
    }
  
  //------------------------------
  // Constraint (6)
  //------------------------------
  constraint=0;
  for (int g=0;g < n_gateways; g++)
    for (int c=0;c < n_clusters; c++)
    { sprintf(str_out,"C6_%d: tauT_%d_%d + mu_%d_%d - tauD_%d_%d <= 0\n",constraint++,g,c,g,c,g,c);
      fputs(str_out,out_file);
    }

  //------------------------------
  // Constraint (7)
  //------------------------------
  constraint=0;
  for (int g=0;g < n_gateways; g++)
    for (int cj=0;cj < n_clusters; cj++)
      for (int ck=0;ck < n_clusters; ck++)
      { sprintf(str_out,"C7_%d: tauD_%d_%d - tauA_%d_%d + 1000000 la_%d_%d_%d <= %d\n",constraint++,g,cj,g,ck,g,cj,ck,1000000-mat_delays[(cj*n_clusters)+ck]);
        fputs(str_out,out_file);
      }
    
  //------------------------------
  // Constraint (8)
  //------------------------------
  constraint=0;
  for (int g=0;g < n_gateways; g++)
  { sprintf(str_out,"C8_%d: tauMAX - tauA_%d_%d >= 0\n",constraint++,g,n_clusters-1);
    fputs(str_out,out_file);
  }

  //------------------------------
  // Constraint (9)
  //------------------------------
  constraint=0;
  for (int g=0;g < n_gateways; g++)
    for (int cj=0;cj < n_clusters; cj++)
      if (!Empty(&mat_clusters[cj*n_devices],n_devices))
      { for (int i=0;i < n_devices; i++)
        { sprintf(str_out,"C9_%d: ",constraint++);
          fputs(str_out,out_file);
          sprintf(str_out,"+ rho_%d_%d_%d ",g,cj,i);
          fputs(str_out,out_file);          
          for (int ck=0;ck < n_clusters; ck++)
          { sprintf(str_out,"- %d la_%d_%d_%d ",mat_clusters[cj*n_devices+i],g,cj,ck);
            fputs(str_out,out_file);
          }
          fputs("<= 0\n",out_file);
        }        
      }
      
  //------------------------------
  // Constraint (10)
  //------------------------------
  constraint=0;
  for (int i=0;i < n_devices; i++)
  { sprintf(str_out,"C10_%d: ",constraint++);
    fputs(str_out,out_file);
    for (int g=0;g < n_gateways; g++)
      for (int c=0;c < n_clusters; c++)
      if (mat_clusters[c*n_devices+i])
      { sprintf(str_out,"+ rho_%d_%d_%d ",g,c,i);
        fputs(str_out,out_file);
      }
    sprintf(str_out,"= 1\n");
    fputs(str_out,out_file);
  }

  //------------------------------
  // Constraint (11)
  //------------------------------
  constraint=0;
  for (int g=0;g < n_gateways; g++)
    for (int cj=0;cj < n_clusters; cj++)
      for (int i=0;i < n_devices; i++)
      if (mat_clusters[cj*n_devices+i])
      { sprintf(str_out,"C11_%d: ",constraint++);
        fputs(str_out,out_file);
        for (int ck=0;ck < n_clusters; ck++)
          if (vec_edge[ck]==1)
          { sprintf(str_out,"+ phi_%d_%d_%d_%d ",g,ck,cj,i);
            fputs(str_out,out_file);
          }
        sprintf(str_out,"- rho_%d_%d_%d = 0\n",g,cj,i);
        fputs(str_out,out_file);
      }
  
  //------------------------------
  // Constraint (12)
  //------------------------------
  constraint=0;
  for (int g=0;g < n_gateways; g++)
    for (int ck=0;ck < n_clusters; ck++)
      if (vec_edge[ck]==1)
      { sprintf(str_out,"C12_%d: ",constraint++);
        fputs(str_out,out_file);
        for (int cj=0;cj < n_clusters; cj++)
          for (int i=0;i < n_devices; i++)
          if (mat_clusters[cj*n_devices+i])
          { sprintf(str_out,"+ phi_%d_%d_%d_%d ",g,ck,cj,i);
            fputs(str_out,out_file);
          }
        for (int cj=0;cj < n_clusters; cj++)
        { sprintf(str_out,"- %d la_%d_%d_%d ",n_clusters*n_devices,g,cj,ck);
          fputs(str_out,out_file);
        }
        sprintf(str_out,"<= 0\n");
        fputs(str_out,out_file);
      }

  //------------------------------
  // Constraint (13)
  //------------------------------
  constraint=0;

  for (int g=0;g < n_gateways; g++)
    for (int ck=0;ck < n_clusters; ck++)
      if (vec_edge[ck]==1)
      { for (int cj=0;cj < n_clusters; cj++)
          for (int i=0;i < n_devices; i++)
          if (mat_clusters[cj*n_devices+i])
          { sprintf(str_out,"C13_%d: theta_%d_%d_%d_%d - phi_%d_%d_%d_%d >= 0\n",constraint++,g,ck,cj,i,g,ck,cj,i);
            fputs(str_out,out_file);
          }
      }
  //------------------------------
  // Constraint (14)
  //------------------------------
  constraint=0;

  for (int g=0;g < n_gateways; g++)
    for (int ck=0;ck < n_clusters; ck++)
      if (vec_edge[ck]==1)
      { for (int cj=0;cj < n_clusters; cj++)
          for (int i=0;i < n_devices; i++)
          if (mat_clusters[cj*n_devices+i])
          { sprintf(str_out,"C14_%d: tauT_%d_%d - tauT_%d_%d + 1000000 phi_%d_%d_%d_%d <= %d\n",constraint++,g,ck,g,cj,g,ck,cj,i,1000000+freshness);
            fputs(str_out,out_file);
          }
      }

  //------------------------------
  // Constraint (15)
  //------------------------------
  constraint=0;
  for (int g=0;g < n_gateways; g++)
    for (int cj=0;cj < n_clusters; cj++)
    { sprintf(str_out,"C15_%d: mu_%d_%d ",constraint++,g,cj);
      fputs(str_out,out_file);
      for (int i=0;i < n_devices; i++)
        if (mat_clusters[cj*n_devices+i])
          { sprintf(str_out,"- %d rho_%d_%d_%d ",mat_spectrum[(i*n_clusters)+cj],g,cj,i);
            fputs(str_out,out_file);
            for (int ck=0;ck < n_clusters; ck++)
            {  sprintf(str_out,"- %d phi_%d_%d_%d_%d ",mat_spectrum[(i*n_clusters)+cj],g,cj,ck,i);
               fputs(str_out,out_file);               
            }
          }
      sprintf(str_out,">= 0\n");
      fputs(str_out,out_file);
    }
  //------------------------------ 
  // Puts bounds (on general variables) and defines binary variables
  //------------------------------
  
  sprintf(str_out,"Bounds\n");
  fputs(str_out,out_file);
  for (int g=0;g < n_gateways; g++)
    for (int c=0;c < n_clusters; c++)
    { sprintf(str_out,"tauA_%d_%d >= 0\n",g,c);
      fputs(str_out,out_file);
      sprintf(str_out,"tauD_%d_%d >= 0\n",g,c);
      fputs(str_out,out_file);
      sprintf(str_out,"tauT_%d_%d >= 0\n",g,c);
      fputs(str_out,out_file);
      sprintf(str_out,"mu_%d_%d >= 0\n",g,c);
      fputs(str_out,out_file);
    }
  sprintf(str_out,"tauMAX >= 0\n");
  fputs(str_out,out_file);
  
  for (int i=0;i < n_devices; i++)
    for (int g=0;g < n_gateways; g++)
      for (int c=0;c < n_clusters; c++)
      if (mat_clusters[c*n_devices+i])
      { sprintf(str_out,"0 <= rho_%d_%d_%d <= 1\n",g,c,i);
        fputs(str_out,out_file);
      }
 
  for (int g=0;g < n_gateways; g++)
    for (int ck=0;ck < n_clusters; ck++)
      if (vec_edge[ck]==1)
      { for (int cj=0;cj < n_clusters; cj++)
          for (int i=0;i < n_devices; i++)
          if (mat_clusters[cj*n_devices+i])
          { sprintf(str_out,"0 <= phi_%d_%d_%d_%d <= 1\n",g,ck,cj,i);
            fputs(str_out,out_file);
          }   
      }
            
  sprintf(str_out,"Binary\n");
  fputs(str_out,out_file);
  for (int g=0;g < n_gateways; g++)
  { for (int c=1;c < n_clusters; c++)
    { sprintf(str_out,"la_%d_%d_%d\n",g,0,c);  
      fputs(str_out,out_file);
    }
  
    for (int c=0;c < n_clusters-1; c++)
    { sprintf(str_out,"la_%d_%d_%d\n",g,c,n_clusters-1);  
      fputs(str_out,out_file);
    }
  
    for (int cj=1;cj < n_clusters-1; cj++)
      for (int ck=0;ck < n_clusters; ck++)
      {  sprintf(str_out,"la_%d_%d_%d\n",g,ck,cj);  
         fputs(str_out,out_file);
         sprintf(str_out,"la_%d_%d_%d\n",g,cj,ck);  
         fputs(str_out,out_file);
      }
    for (int ck=0;ck < n_clusters; ck++)
      if (vec_edge[ck]==1)
      { for (int cj=0;cj < n_clusters; cj++)
          for (int i=0;i < n_devices; i++)
          if (mat_clusters[cj*n_devices+i])
          { sprintf(str_out,"phi_%d_%d_%d_%d\n",g,ck,cj,i);
            fputs(str_out,out_file);
          }   
      }
  }

  //-----------------
  // puts end in file
  //-----------------
  sprintf(str_out,"end\n");
  fputs(str_out,out_file);

  free (mat_clusters);
  free (vec_edge);
  free (mat_delays);
  free (mat_spectrum);  
  fclose(in_clusters);
  fclose(in_edge);
  fclose(in_delays);
  fclose(in_spectrum);
  fclose(out_file);
}
