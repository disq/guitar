#include "programs.h"
#include "gladesrc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

/*
{
  OPEN,
  CREATE,
  ADD,
  DELETE,
  VIEW,
  EXTRACT
}
*/

void reopen(tarstruct *tar)
{
char orgfile[512];
char param_comp[256];

/*gint curpage;

curpage=_NOTEBOOK_GET_CURRENT_PAGE_(GTK_NOTEBOOK(notebook));
*/

strcpy(orgfile,tar->ourfile);
strcpy(param_comp,tar->param_comp);
close_file();
if (files_open_real(orgfile)==0)
  {
  strcpy(tartemp->param_comp,param_comp);
  tar=tartemp;
  }

/*gtk_notebook_set_page(GTK_NOTEBOOK(notebook),tarcount);*/
}

int check4errors(char *fn, char *errtext)
{
FILE *f;
int ret;

  f=fopen(fn,"r");
  if (f!=NULL)
    {
	fseek(f,0,SEEK_END);
  	if (ftell(f)>1)
	  {
  	  fclose(f);  
  	  create_viewwin(fn,errtext);
	  ret=1;
	  }
  	else ret=0;
	}
	else ret=2;

return (ret);
}


void add_file_to_list(tarstruct *tar, gchar *s, gint row)
{
gpointer *x;

  x=(gpointer)g_strdup(s);
  tar->files=g_list_append(tar->files,x);
  gtk_clist_set_row_data(GTK_CLIST(tar->clist),row,x);
}

void remove_last_file_from_list(tarstruct *tar)
{
GList *g;
gpointer *x;

  g=g_list_last(tar->files);
  if (g!=NULL)
    {
    x=g->data;
    tar->files=g_list_remove(tar->files,x);
    g_free(x);
    }
}


int _rar(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p)
{
pid_t pid;
FILE *f;
int ret=0;

  if (func==CREATE) func=ADD;

  if (func==OPEN)
	{
	char d[7][256];
	gchar *add[7];
	unsigned int i,lastrow=0,lastrow2=0;
	char *tmp1,*tmp2;
	char ln[1024];
	char dumb[512];
	unsigned long totalbytes=0;
	unsigned long totalfiles=0;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"unrar v -c- -p- \"%s\" >%s",fn,tmp2);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
	  }
	wait_child_finish();
	pbar_init();
  
	gtk_clist_clear(GTK_CLIST(tar->clist));

	f=fopen(tmp2,"r");
	gtk_clist_freeze(GTK_CLIST(tar->clist));

    readl(f,ln);
	readl(f,ln);
	readl(f,ln);
	readl(f,ln);
	readl(f,ln);
	readl(f,ln);
	readl(f,ln);
	readl(f,ln);
/* unrar: 8 lines, rar: 9 lines */

	while (!feof(f))  
	  {
	  for(i=0;i<7;i++) strcpy(d[i],"");
	  strcpy(ln,"");
	  strcpy(d[5],"");

	  pbar_update();

	  readl(f,d[5]);
	  readl(f,ln);

	  memmove(d[5],&d[5][1],strlen(d[5]));
	
	if ((strcmp(ln,"")!=0) && (strcmp(d[5],"")!=0))
	  {
	  strcpy(d[1],"");
	  strcpy(d[6],"");
	  sscanf(ln,"%s %s %s %s %s %s %s %s %s",d[2],dumb,dumb,d[3],d[4],d[0],dumb,dumb,dumb);
/* 123456789012*/
/*11559     3664  31% 27-02-99 17:23 -rw-r--r-- B5F5C4BF m3c 2.0*/

				 
	  add[0]=d[0];
	  add[1]=d[1];
	  add[2]=d[2];
	  add[3]=d[3];
	  add[4]=d[4];
	  add[5]=d[5];
	  add[6]=d[6];
	  lastrow2=lastrow;
  	  lastrow=gtk_clist_append(GTK_CLIST(tar->clist),add);
	  add_file_to_list(tar,add[5],lastrow);
	  
	  totalbytes+=atoi(d[2]);
	  totalfiles++;
	  }
	}
	
	totalfiles--;

#ifdef _CLIST_SET_COLUMN_VISIBILITY_
    gtk_clist_set_column_visibility(GTK_CLIST(tar->clist),1,FALSE);
    gtk_clist_set_column_visibility(GTK_CLIST(tar->clist),6,FALSE);
#endif

    gtk_clist_remove(GTK_CLIST(tar->clist),lastrow);
	remove_last_file_from_list(tar);

    gtk_clist_thaw(GTK_CLIST(tar->clist));
    fclose(f);
    PopStatusbar();
    remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
    pbar_deinit();
	{
	char buf[4096];
	  sprintf(buf,N_("Ok - %lu bytes, %lu files"),totalbytes,totalfiles);
	  PushStatusbar(buf);
	}
	ret=0;
	}
  else if (func==ADD)
	{
	char *tmp1,*tmp2;
	char *listfile;
	
	listfile=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");

	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"rar a -c- -p- %s \"%s\" @%s 2> %s",param_ratio,fn,listfile,tmp2);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	ret=check4errors(tmp2,N_("Error(s) occured while creating/adding file"));

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	}

  else if (func==DELETE)
	{
	char *tmp1,*tmp2,*tmp3;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	tmp3=tempnam(NULL,"guitar");
	
	write_selection_to_file(tar,tmp1);

	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp2,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"rar d -c- -p- \"%s\" @%s 2> %s",fn,tmp1,tmp3);
  	  fclose(f);

	  chmod(tmp2,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp2,tmp2,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	ret=check4errors(tmp3,N_("Error(s) occured while deleting file"));

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	remove(tmp3); free(tmp3);
	}

  else if (func==VIEW)
	{
	char *tmp1,*tmp2,*tmp3;
	char a[512];
	char *filetoview;
	
	filetoview=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
    tmp3=tempnam(NULL,"guitar");

    pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
/*	  fprintf(f,"unrar p -ierr -c- -p- \"%s\" \"%s\" >%s 2> %s",fn,filetoview,tmp2,tmp3);
*/	  fprintf(f,"unrar p -inul -c- -p- \"%s\" \"%s\" >%s 2> %s",fn,filetoview,tmp2,tmp3);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

/*
	check4errors(tmp3,N_("Archiver output (may not be an error)"));

	while(gtk_events_pending()) gtk_main_iteration();
*/

    sprintf(a,"View - %s/%s",tar->ourfile,filetoview);
    create_viewwin(tmp2,a);
/*
	while(gtk_events_pending()) gtk_main_iteration();
*/

	ret=0;

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	remove(tmp3); free(tmp3);
	}

  else if (func==EXTRACT)
	{
	char *tmp1,*tmp2,*tmp3;
    char curdir[256];
	char *extractto;
	
	extractto=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	tmp3=tempnam(NULL,"guitar");

	write_selection_to_file(tar, tmp1);

	getcwd(curdir,256);
	chdir(extractto);

	pid=fork();
	if (!pid)
	  {
	  f=fopen(tmp2,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"unrar x -c- -p- \"%s\" @%s 2> %s",fn,tmp1,tmp3);
  	  fclose(f);

	  chmod(tmp2,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp2,tmp2,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	chdir(curdir);

	ret=check4errors(tmp3,N_("Error(s) occured while extracting file"));

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	remove(tmp3); free(tmp3);
	}

if ((ret==0) && ((func==ADD) || (func==DELETE))) reopen(tar);
return(ret);
}




int _lha(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p)
{
pid_t pid;
FILE *f;
int ret=0;

  if (func==CREATE) func=ADD;

  if (func==OPEN)
	{
	char d[7][256];
	gchar *add[7];
	unsigned int l,i;
	char *tmp1,*tmp2;
	char ln[1024];
	char dumb[512];
	unsigned long totalbytes=0;
	unsigned long totalfiles=0;
	int row;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"lha lq \"%s\" >%s",fn,tmp2);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
	  }
	wait_child_finish();
	pbar_init();
  
	gtk_clist_clear(GTK_CLIST(tar->clist));

	f=fopen(tmp2,"r");
	gtk_clist_freeze(GTK_CLIST(tar->clist));

	while (!feof(f))  
	  {
	  for(i=0;i<7;i++) strcpy(d[i],"");
	  strcpy(ln,"");

	  pbar_update();

	  readl(f,ln);
	
	if (strcmp(ln,"")!=0) 
	  {
	  strcpy(d[6],"");
	  sscanf(ln,"%s %s %s %s %s %s %s",d[0],d[1],d[2],dumb,d[3],dumb,d[4]);
/* rw-r--r--   0/0         9 100.0% Feb 26 15:56 eaoirj aerjoi ear oier */


	  l=findstr(ln,d[4]);
	  l+=strlen(d[4])+1;


	  strcat(d[3]," \0");
	  strcat(d[3],dumb);
	  if (d[4][2]!=':')
		{
		strcat(d[3]," \0");
		strcat(d[3],d[4]);
		strcpy(d[4],"unknown");
		}


	  strcpy(d[5],&ln[l]);
		
	  add[0]=d[0];
	  add[1]=d[1];
	  add[2]=d[2];
	  add[3]=d[3];
	  add[4]=d[4];
	  add[5]=d[5];
	  add[6]=d[6];
  	  row=gtk_clist_append(GTK_CLIST(tar->clist),add);
	  add_file_to_list(tar,add[5],row);

	  totalbytes+=atoi(d[2]);
	  totalfiles++;
	  }
	}
	
	totalfiles--;

#ifdef _CLIST_SET_COLUMN_VISIBILITY_
	gtk_clist_set_column_visibility(GTK_CLIST(tar->clist),6,FALSE);
#endif

    gtk_clist_thaw(GTK_CLIST(tar->clist));
    fclose(f);
    PopStatusbar();
    remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
    pbar_deinit();
	{
	char buf[4096];
	  sprintf(buf,N_("Ok - %lu bytes, %lu files"),totalbytes,totalfiles);
	  PushStatusbar(buf);
	}
	ret=0;
	}
  else if (func==ADD)
	{
	char *tmp1,*tmp2;
	char *listfile;
	FILE *g;
	char buf[4096];
	
	listfile=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");

	pid=fork();
	if (!pid)
  	  {
	  g=fopen(listfile,"r");
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
      fprintf(f,"lha a \"%s\"",fn);

	  while(!feof(g))
		{
		strcpy(buf,"");
		readl(g,buf);
		if (strcmp(buf,"")!=0) fprintf(f," \"%s\"",buf);
		}
  	  fclose(g);

      fprintf(f," 2>%s",tmp2);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	ret=check4errors(tmp2,N_("Error(s) occured while creating/adding file"));

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	}

  else if (func==DELETE)
	{
	char *tmp1,*tmp2,*tmp3;
	FILE *g;
	char buf[4096];

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	tmp3=tempnam(NULL,"guitar");
	
	write_selection_to_file(tar,tmp1);

	pid=fork();
	if (!pid)
  	  {
	  g=fopen(tmp1,"r");
	  f=fopen(tmp2,"w");
  	  fprintf(f,"#!/bin/sh\n");

      fprintf(f,"lha d \"%s\"",fn);

	  while(!feof(g))
		{
		strcpy(buf,"");
		readl(g,buf);
		if (strcmp(buf,"")!=0) fprintf(f," \"%s\"",buf);
		}
  	  fclose(g);

      fprintf(f," 2>%s",tmp3);
  	  fclose(f);

	  chmod(tmp2,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp2,tmp2,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	ret=check4errors(tmp3,N_("Error(s) occured while deleting file"));

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	remove(tmp3); free(tmp3);
	}

  else if (func==VIEW)
	{
	char *tmp1,*tmp2,*tmp3;
	char a[512];
	char *filetoview;
	
	filetoview=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
    tmp3=tempnam(NULL,"guitar");

    pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"lha pq \"%s\" \"%s\" >%s 2> %s",fn,filetoview,tmp2,tmp3);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	ret=check4errors(tmp3,N_("Error(s) occured while extracting file"));
	if (ret==0)
	  {
	  sprintf(a,"View - %s/%s",tar->ourfile,filetoview);
	  create_viewwin(tmp2,a);
	  }

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
    remove(tmp3); free(tmp3);
	}

  else if (func==EXTRACT)
	{
	FILE *g;
	char *tmp1,*tmp2,*tmp3;
    char curdir[256];
	char *extractto;
	char singlefile[1024];
	
	extractto=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	tmp3=tempnam(NULL,"guitar");

	write_selection_to_file(tar, tmp1);

	getcwd(curdir,256);
	chdir(extractto);

	pid=fork();
	if (!pid)
	  {
	  f=fopen(tmp2,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"lha x \"%s\" ",fn);
	  g=fopen(tmp1,"r");
	  while (!feof(g))
		{
		readl(g,singlefile);
		if (strcmp(singlefile,"")!=0) fprintf(f,"\"%s\" ",singlefile);
		}
	  fclose(g);
	  fprintf(f," >/dev/null 2>%s",tmp3);
  	  fclose(f);

	  chmod(tmp2,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp2,tmp2,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	chdir(curdir);

	ret=check4errors(tmp3,N_("Error(s) occured while extracting file"));

    remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	remove(tmp3); free(tmp3);
	}

if ((ret==0) && ((func==ADD) || (func==DELETE))) reopen(tar);
return(ret);
}



int _arj(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p)
{
pid_t pid;
FILE *f;
int ret=0;

  if (func==CREATE) func=ADD;

  if (func==OPEN)
	{
	char d[7][256];
	gchar *add[7];
	unsigned int i,lastrow=0,lastrow2=0;
	char *tmp1,*tmp2;
	char ln[1024];
	char dumb[512];
	unsigned long totalbytes=0;
	unsigned long totalfiles=0;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"unarj l \"%s\" >%s",fn,tmp2);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
	  }
	wait_child_finish();
	pbar_init();
  
	gtk_clist_clear(GTK_CLIST(tar->clist));

	f=fopen(tmp2,"r");
	gtk_clist_freeze(GTK_CLIST(tar->clist));

    readl(f,ln);
	readl(f,ln);
	readl(f,ln);
	readl(f,ln);
	readl(f,ln);
	readl(f,ln);

	while (!feof(f))  
	  {
	  for(i=0;i<7;i++) strcpy(d[i],"");
	  strcpy(ln,"");

	  pbar_update();

	  readl(f,ln);
	
	if (strcmp(ln,"")!=0) 
	  {
	  strcpy(d[6],"");
	  sscanf(ln,"%s %s %s %s %s %s %s %s",d[5],d[2],dumb,dumb,d[3],d[4],dumb,dumb);
/*FILE_ID.DIZ          66         63 0.955 99-01-26 23:27:02 C3F5A217      B 1*/

	  add[0]=d[0];
	  add[1]=d[1];
	  add[2]=d[2];
	  add[3]=d[3];
	  add[4]=d[4];
	  add[5]=d[5];
	  add[6]=d[6];
	  lastrow2=lastrow;
  	  lastrow=gtk_clist_append(GTK_CLIST(tar->clist),add);
	  add_file_to_list(tar,add[5],lastrow);
	  totalbytes+=atoi(d[2]);
	  totalfiles++;
	  }
	}
	
	totalfiles--;

#ifdef _CLIST_SET_COLUMN_VISIBILITY_
    gtk_clist_set_column_visibility(GTK_CLIST(tar->clist),0,FALSE);
    gtk_clist_set_column_visibility(GTK_CLIST(tar->clist),1,FALSE);
    gtk_clist_set_column_visibility(GTK_CLIST(tar->clist),6,FALSE);
#endif

    gtk_clist_remove(GTK_CLIST(tar->clist),lastrow);
    gtk_clist_remove(GTK_CLIST(tar->clist),lastrow2);
	remove_last_file_from_list(tar);
	remove_last_file_from_list(tar);
    gtk_clist_thaw(GTK_CLIST(tar->clist));
    fclose(f);
    PopStatusbar();
    remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
    pbar_deinit();
	{
	char buf[4096];
	  sprintf(buf,N_("Ok - %lu bytes, %lu files"),totalbytes,totalfiles);
	  PushStatusbar(buf);
	}
	ret=0;
	}
  else if (func==ADD)
	{
	return(3);
	}

  else if (func==DELETE)
	{
	return(3);
	}

  else if (func==VIEW)
	{
	return(3);
	}

  else if (func==EXTRACT)
	{
	char *tmp1,*tmp2;
    char curdir[256];
	char *extractto;
	
	extractto=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");

	getcwd(curdir,256);
	chdir(extractto);

	pid=fork();
	if (!pid)
	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"unarj x \"%s\" 2>%s",fn,tmp2);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	chdir(curdir);

	ret=check4errors(tmp2,N_("Error(s) occured while extracting file"));

    remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	}

/* if ((ret==0) && ((func==ADD) || (func==DELETE))) reopen(tar); */
return(ret);
}





int _zip(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p)
{
pid_t pid;
FILE *f;
int ret=0;

  if (func==CREATE) func=ADD;

  if (func==OPEN)
	{
	char d[7][256];
	gchar *add[7];
	unsigned int l,i,lastrow=0;
	char *tmp1,*tmp2;
	char ln[1024];
	char dumb[512];
	unsigned long totalbytes=0;
	unsigned long totalfiles=0;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"unzip -Zs \"%s\" >%s",fn,tmp2);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
	  }
	wait_child_finish();
	pbar_init();
  
	gtk_clist_clear(GTK_CLIST(tar->clist));

	f=fopen(tmp2,"r");
	gtk_clist_freeze(GTK_CLIST(tar->clist));

	readl(f,ln);

	while (!feof(f))  
	  {
	  for(i=0;i<7;i++) strcpy(d[i],"");
	  strcpy(ln,"");

	  pbar_update();

	  readl(f,ln);
	
	if (strcmp(ln,"")!=0) 
	  {
	  strcpy(d[1],"");
	  sscanf(ln,"%s %s %s %s %s %s %s %s",d[0],dumb,dumb,d[2],dumb,dumb,d[3],d[4]);
/* -rw-r--r--  2.1 unx     313 tx defN 18-Mar-98 17:24 id3ren/src/Makefile.win32 */

	  l=findstr(ln,d[4]);
	  l+=strlen(d[4])+1;

	  strcpy(d[5],&ln[l]);
		
	  add[0]=d[0];
	  add[1]=d[1];
	  add[2]=d[2];
	  add[3]=d[3];
	  add[4]=d[4];
	  add[5]=d[5];
	  add[6]=d[6];
  	  lastrow=gtk_clist_append(GTK_CLIST(tar->clist),add);
  	  add_file_to_list(tar,add[5],lastrow);
	  totalbytes+=atoi(d[2]);
	  totalfiles++;
	  }
	}
	
	totalfiles--;

#ifdef _CLIST_SET_COLUMN_VISIBILITY_
    gtk_clist_set_column_visibility(GTK_CLIST(tar->clist),1,FALSE);
    gtk_clist_set_column_visibility(GTK_CLIST(tar->clist),6,FALSE);
#endif

    gtk_clist_remove(GTK_CLIST(tar->clist),lastrow);
	remove_last_file_from_list(tar);
    gtk_clist_thaw(GTK_CLIST(tar->clist));
    fclose(f);
    PopStatusbar();
    remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
    pbar_deinit();
	{
	char buf[4096];
	  sprintf(buf,N_("Ok - %lu bytes, %lu files"),totalbytes,totalfiles);
	  PushStatusbar(buf);
	}
	ret=0;
	}
  else if (func==ADD)
	{
	char *tmp1,*tmp2;
	char *listfile;
	
	listfile=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");

	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"cat %s | zip %s \"%s\" -@ 2> %s",listfile,param_ratio,fn,tmp2);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	ret=check4errors(tmp2,N_("Error(s) occured while creating/adding file"));

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	}

  else if (func==DELETE)
	{
	char *tmp1,*tmp2,*tmp3;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	tmp3=tempnam(NULL,"guitar");
	
	write_selection_to_file(tar,tmp1);

	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp2,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"cat %s | zip -d \"%s\" -@ 2> %s",tmp1,fn,tmp3);
  	  fclose(f);

	  chmod(tmp2,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp2,tmp2,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	ret=check4errors(tmp3,N_("Error(s) occured while deleting file"));

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	remove(tmp3); free(tmp3);
	}

  else if (func==VIEW)
	{
	char *tmp1,*tmp2,*tmp3;
	char a[512];
	char *filetoview;
	
	filetoview=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
    tmp3=tempnam(NULL,"guitar");

    pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"unzip -p \"%s\" \"%s\" >%s 2> %s",fn,filetoview,tmp2,tmp3);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	ret=check4errors(tmp3,N_("Error(s) occured while extracting file"));
	if (ret==0)
	  {
	  sprintf(a,"View - %s/%s",tar->ourfile,filetoview);
	  create_viewwin(tmp2,a);
	  }

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
    remove(tmp3); free(tmp3);
	}

  else if (func==EXTRACT)
	{
	FILE *g;
	char *tmp1,*tmp2,*tmp3;
    char curdir[256];
	char *extractto;
	char singlefile[1024];
	
	extractto=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	tmp3=tempnam(NULL,"guitar");

	write_selection_to_file(tar, tmp1);

	getcwd(curdir,256);
	chdir(extractto);

	pid=fork();
	if (!pid)
	  {
	  f=fopen(tmp2,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"unzip \"%s\" ",fn);
	  g=fopen(tmp1,"r");
	  while (!feof(g))
		{
		readl(g,singlefile);
		if (strcmp(singlefile,"")!=0) fprintf(f,"\"%s\" ",singlefile);
		}
	  fclose(g);
	  fprintf(f," >/dev/null 2>%s",tmp3);
  	  fclose(f);

	  chmod(tmp2,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp2,tmp2,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	chdir(curdir);

	ret=check4errors(tmp3,N_("Error(s) occured while extracting file"));

    remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	remove(tmp3); free(tmp3);
	}

if ((ret==0) && ((func==ADD) || (func==DELETE))) reopen(tar);
return(ret);
}

int _tarz(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p)
{
return(_tarX(func,tar,fn,"compress","-c","-c","-d",param_ratio,p));
}

int _tarbz2(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p)
{
return(_tarX(func,tar,fn,"bzip2","-c","","-d",param_ratio,p));
}

int _targz(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p)
{
return(_tarX(func,tar,fn,"gzip","-c","-c","-d",param_ratio,p));
}

int _tarX(archivefunctionType func, tarstruct *tar, char *fn, char *compprog, char *param_stdout, char *param_stdout_when_decomp, char *param_decomp, char *param_ratio, gpointer *p)
{
char buf[4096];
char *tmp1,*tmp2,*tmp3;
int ret=0;

  if ((func==CREATE) || (func==ADD) || (func==DELETE))
	{
  	tmp1=tempnam(NULL,"guitar");
  	tmp2=tempnam(NULL,"guitar");
  	tmp3=tempnam(NULL,"guitar");

	if (compprog==NULL) compprog="gzip";
	
	if (func!=CREATE)
	  {
	  sprintf(buf,"cat %s | %s %s %s > %s 2> %s",fn,compprog,param_stdout_when_decomp,param_decomp,tmp1,tmp2);
	  system(buf);
	  ret=check4errors(tmp2,N_("Error(s) decompressing file"));
	  } else ret=0;
	if (ret==0)
	  {
	  ret=_tar(func,tar,tmp1,"",p,FALSE);
	  if (ret==0)
		{
		sprintf(buf,"%s %s %s %s > %s 2> %s",compprog,param_ratio,param_stdout,tmp1,tmp3,tmp2);
		system(buf);
		ret=check4errors(tmp2,N_("Error(s) compressing file"));
		if (ret==0)
		  {
  		  sprintf(buf,"mv -f -b \"%s\" \"%s\" 2> %s",tmp3,fn,tmp2);
		  system(buf);
  		  ret=check4errors(tmp2,N_("Error(s) moving file after compression"));
		  }
		}
	}
  remove(tmp1); free(tmp1);
  remove(tmp2); free(tmp2);
  remove(tmp3); free(tmp3);
  }
  else
	{
	sprintf(buf,"cat %s | %s %s %s |",fn,compprog,param_decomp,param_stdout_when_decomp);
    ret=_tar(func,tar,"",buf,p,FALSE);
	}

if ((ret==0) && ((func==ADD) || (func==DELETE))) reopen(tar);
return(ret);
}

/*
 open: p=null
 create: p=listfile
 add: p=listfile
 delete: p=null
 view: p=filetoview
 extract: p=extractdir
*/
int _tar(archivefunctionType func, tarstruct *tar, char *fnf, char *pipe, gpointer *p, gboolean update)
{
pid_t pid;
FILE *f=NULL;
int ret=0;
char fn[514];

if (strcmp(fnf,"")==0)
  {
  strcpy(fn,"-");
  }
else
  {
  sprintf(fn,"\"%s\"",fnf);
  }

  if (func==OPEN)
	{
	char d[7][256];
	gchar *add[7];
	unsigned int l,i,lastrow=0;
	char *tmp1,*tmp2,*tmp3;
	char ln[1024];
	unsigned long totalbytes=0;
	unsigned long totalfiles=0;
	
	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	tmp3=tempnam(NULL,"guitar");
	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"%s tar vtf %s >%s 2>%s",pipe,fn,tmp2,tmp3);
      fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
	  exit(0);
  	  }
	wait_child_finish();
	
	ret=check4errors(tmp3,N_("Error(s) opening file"));

	if (ret==0)
	  {
	pbar_init();

	gtk_clist_clear(GTK_CLIST(tar->clist));

	f=fopen(tmp2,"r");
	gtk_clist_freeze(GTK_CLIST(tar->clist));

	while (!feof(f))  
	  {
	  for(i=0;i<7;i++) strcpy(d[i],"");
	  strcpy(ln,"");

	  pbar_update();

	  readl(f,ln);
	
	  if (strcmp(ln,"")!=0) sscanf(ln,"%s %s %s %s %s",d[0],d[1],d[2],d[3],d[4]);

	  l=findstr(ln,d[4]);
	  l+=strlen(d[4])+1;

	  if (strlen(d[3])==3) /* alert! */
		{
		char a[3][256];
		for(i=0;i<3;i++) strcpy(a[i],"");
  		sscanf(ln,"%s %s %s %s %s %s %s",d[0],d[1],d[2],a[0],a[1],d[4],a[2]);
		sprintf(d[3],"%s-%s-%s",a[1],a[0],a[2]);
	  
		l=findstr(ln,a[2]);
		l+=strlen(a[2])+1;
		}

/*
-rw-r--r-- owner/group     13001 1997-07-12 15:29 myfilename
-rw-r--r-- owner/group     13001 Dec 12 15:29 1997 myfilename
0          1               2     3   4
*/

	  i=findstr(&ln[l],"->");
	  if (i==-1) strcpy(d[5],&ln[l]);
	  else
		{
  		unsigned int j;
		
	  	strcpy(d[6],&ln[l+i+2]);
		for(j=0;j<i;j++) d[5][j]=ln[l+j];
		d[5][i]=0;
	  	}
		
	  add[0]=d[0];
	  add[1]=d[1];
	  add[2]=d[2];
	  add[3]=d[3];
	  add[4]=d[4];
	  add[5]=d[5];
	  add[6]=d[6];
  	  lastrow=gtk_clist_append(GTK_CLIST(tar->clist),add);
  	  add_file_to_list(tar,add[5],lastrow);
	  totalbytes+=atoi(d[2]);
	  totalfiles++;
	  }

	totalfiles--;

	gtk_clist_remove(GTK_CLIST(tar->clist),lastrow);
	remove_last_file_from_list(tar);
	gtk_clist_thaw(GTK_CLIST(tar->clist));
	fclose(f);
	PopStatusbar();
	pbar_deinit();
	{
	char buf[4096];
	  sprintf(buf,N_("Ok - %lu bytes, %lu files"),totalbytes,totalfiles);
	  PushStatusbar(buf);
	}

	  }
	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	remove(tmp3); free(tmp3);
	}

  else if (func==CREATE)
	{
	char *tmp1, *tmp2;
	char *listfile;
	
	listfile=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");

	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"%s tar vcf %s --files-from=%s 2> %s",pipe,fn,listfile,tmp1);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
  	  }
	wait_child_finish();
	
	tar_lameworkaround(tmp1,tmp2);

	ret=check4errors(tmp2,N_("Error(s) occured while creating archive"));

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	}

  else if (func==ADD)
	{
	char *tmp1, *tmp2;
	char *listfile;
	
	listfile=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");

	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"%s tar vrf %s --files-from=%s 2> %s",pipe,fn,listfile,tmp1);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	tar_lameworkaround(tmp1,tmp2);

	ret=check4errors(tmp2,N_("Error(s) occured while adding file"));

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	}

  else if (func==DELETE)
	{
	char *tmp1,*tmp2,*tmp3;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	tmp3=tempnam(NULL,"guitar");

	write_selection_to_file(tar,tmp1);

	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp2,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"%s tar vf %s --delete --files-from=%s 2>%s",pipe,fn,tmp1,tmp3);
  	  fclose(f);

	  chmod(tmp2,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp2,tmp2,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	ret=check4errors(tmp3,N_("Error(s) occured while deleting file(s)"));

	PopStatusbar();
	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	remove(tmp3); free(tmp3);
	}

  else if (func==VIEW)
	{
	char *tmp1,*tmp2,*tmp3;
	char a[512];
	char *filetoview;
	
	filetoview=(char *)p;

	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	tmp3=tempnam(NULL,"guitar");

	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp1,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"%s tar xOf %s --absolute-names \"%s\" >%s 2> %s",pipe,fn,filetoview,tmp2,tmp3);
  	  fclose(f);

	  chmod(tmp1,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp1,tmp1,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	ret=check4errors(tmp3,N_("Error(s) occured while extracting file"));
	if (ret==0)
	  {
	  sprintf(a,"View - %s/%s",tar->ourfile,filetoview);
	  create_viewwin(tmp2,a);
	  }

	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	remove(tmp3); free(tmp3);
	}

  else if (func==EXTRACT)
	{
    char *tmp1,*tmp2,*tmp3, *tmp4;
	char curdir[256];
	char *extractto;
	
	extractto=(char *)p;
	
	tmp1=tempnam(NULL,"guitar");
	tmp2=tempnam(NULL,"guitar");
	tmp3=tempnam(NULL,"guitar");
	tmp4=tempnam(NULL,"guitar");

	write_selection_to_file(tar, tmp1);

	getcwd(curdir,256);
	chdir(extractto);

	pid=fork();
	if (!pid)
  	  {
	  f=fopen(tmp2,"w");
  	  fprintf(f,"#!/bin/sh\n");
	  fprintf(f,"%s tar xf %s --files-from=%s >%s 2>%s",pipe,fn,tmp1,tmp3,tmp3);
  	  fclose(f);

	  chmod(tmp2,S_IRUSR | S_IWUSR | S_IXUSR);
	  execlp(tmp2,tmp2,NULL);
  	  exit(0);
  	  }
	wait_child_finish();

	chdir(curdir);

	tar_lameworkaround(tmp3,tmp4);

	ret=check4errors(tmp4,N_("Error(s) occured while extracting file"));
  
	remove(tmp1); free(tmp1);
	remove(tmp2); free(tmp2);
	remove(tmp3); free(tmp3);
	remove(tmp4); free(tmp4);
	}

if ((ret==0) && (update) && ((func==ADD) || (func==DELETE))) reopen(tar);
return(ret);
}



void tar_lameworkaround(char *src, char *dest)
{
FILE *f,*g;
char line[4096];

  f=fopen(src,"r");
  g=fopen(dest,"w");

  strcpy(line,"");

  while (!feof(f))  
    {
    readl(f, line);
    if (strcmp(line,"tar: Removing leading `/' from absolute path names in the archive")!=0) fprintf(g,"%s",line);
    }
  fclose(f);
  fclose(g);
}
