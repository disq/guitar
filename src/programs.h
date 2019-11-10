#ifndef _PROGRAMS_H_

#include "common.h"
#include "main.h"

void reopen(tarstruct *tar);
int check4errors(char *fn, char *errtext);
void add_file_to_list(tarstruct *tar, gchar *s, gint row);
void remove_last_file_from_list(tarstruct *tar);
int _rar(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p);
int _lha(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p);
int _arj(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p);
int _zip(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p);
int _tarz(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p);
int _tarbz2(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p);
int _targz(archivefunctionType func, tarstruct *tar, char *fn, char *param_ratio, gpointer *p);
int _tarX(archivefunctionType func, tarstruct *tar, char *fn, char *compprog, char *param_stdout, char *param_stdout_when_decomp, char *param_decomp, char *param_ratio, gpointer *p);
/*
 open: p=null
 create: p=listfile
 add: p=listfile
 delete: p=null
 view: p=filetoview
 extract: p=extractdir
*/
int _tar(archivefunctionType func, tarstruct *tar, char *fnf, char *pipe, gpointer *p, gboolean update);
void tar_lameworkaround(char *src, char *dest);

#endif