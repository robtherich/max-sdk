#include "jit.common.h"

typedef struct _jit_matrixtests
{
	t_object		ob;	// t_object member must be first

	// class data
	long			mylong;
} t_jit_matrixtests;

void *_jit_matrixtests_class;

t_jit_err jit_matrixtests_init(void);
t_jit_matrixtests *jit_matrixtests_new(void);
void jit_matrixtests_free(t_jit_matrixtests *x);
t_jit_err jit_matrixtests_matrix_calc(t_jit_matrixtests *x, void *inputs, void *outputs);

t_jit_err jit_matrixtests_init(void)
{
	void *mop;

	_jit_matrixtests_class = jit_class_new("jit_matrixtests",(method)jit_matrixtests_new,(method)jit_matrixtests_free, sizeof(t_jit_matrixtests),0L);

	//add mop
	mop = jit_object_new(_jit_sym_jit_mop,0,1);	// 0 inputs, 1 output
	jit_class_addadornment(_jit_matrixtests_class,mop);

	//add methods
	jit_class_addmethod(_jit_matrixtests_class, (method)jit_matrixtests_matrix_calc, "matrix_calc", A_CANT, 0L);

	jit_class_register(_jit_matrixtests_class);

	return JIT_ERR_NONE;
}

t_jit_err jit_matrixtests_matrix_calc(t_jit_matrixtests *x, void *inputs, void *outputs)
{
	t_jit_err err=JIT_ERR_NONE;
	long out_savelock;
	t_jit_matrix_info out_minfo;
	char *out_bp;
	long i,j, k, width, height, planecount;
	void *out_matrix;

	out_matrix 	= jit_object_method(outputs,_jit_sym_getindex,0);

	if (x && out_matrix) {
		out_savelock = (long) jit_object_method(out_matrix,_jit_sym_lock,1);

		jit_object_method(out_matrix,_jit_sym_getinfo,&out_minfo);

		jit_object_method(out_matrix,_jit_sym_getdata,&out_bp);

		if (!out_bp) { err=JIT_ERR_INVALID_OUTPUT; goto out;}

		width = out_minfo.dim[0];
		height = out_minfo.dim[1];
		planecount = out_minfo.planecount;

		if (out_minfo.type == _jit_sym_char) {
			for (i = 0; i < height; i++) {
				uchar *cop;
				cop = (uchar*)(out_bp + i * out_minfo.dimstride[1]);

				for(j = 0; j < width; j++) {
					for(k = 0; k < planecount; k++) {
						*cop = j;
						cop++;
					}
				}
			}
		}
		else if (out_minfo.type == _jit_sym_float32) {
			for (i = 0; i < height; i++) {
				float *fop;
				fop = (float*)(out_bp + i * out_minfo.dimstride[1]);

				for(j = 0; j < width; j++) {
					for(k = 0; k < planecount; k++) {
						*fop = ((float)j) / 255.;
						fop++;
					}
				}
			}
		}

	}
	else {
		return JIT_ERR_INVALID_PTR;
	}

out:
	jit_object_method(out_matrix,_jit_sym_lock,out_savelock);
	return err;
}

t_jit_matrixtests *jit_matrixtests_new(void)
{
	t_jit_matrixtests *x;

	if ((x=(t_jit_matrixtests *)jit_object_alloc(_jit_matrixtests_class))) {
		x->mylong = 9999;
		// create stuff here
	} else {
		x = NULL;
	}
	return x;
}

void jit_matrixtests_free(t_jit_matrixtests *x)
{
	//free up stuff here
}
