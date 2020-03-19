#include "jit.common.h"
#include "max.jit.mop.h"

typedef struct _max_jit_matrixtests
{
	t_object		ob;
	void			*obex;
} t_max_jit_matrixtests;

BEGIN_USING_C_LINKAGE

t_jit_err jit_matrixtests_init(void);

void *max_jit_matrixtests_new(t_symbol *s, long argc, t_atom *argv);
void max_jit_matrixtests_free(t_max_jit_matrixtests *x);
void max_jit_matrixtests_outputmatrix(t_max_jit_matrixtests *x);

END_USING_C_LINKAGE

static void *max_jit_matrixtests_class = NULL;

void ext_main(void *r)
{
	t_class *max_class, *jit_class;

	// initialize the jitter object class
	jit_matrixtests_init();

	max_class = class_new("jit.matrixtests", (method)max_jit_matrixtests_new, (method)max_jit_matrixtests_free, sizeof(t_max_jit_matrixtests), NULL, A_GIMME, 0);
	max_jit_class_obex_setup(max_class, calcoffset(t_max_jit_matrixtests, obex));

	jit_class = jit_class_findbyname(gensym("jit_matrixtests"));

	// attrs & methods for name, type, dim, planecount, bang, outputmatrix, etc
	max_jit_class_mop_wrap(max_class, jit_class, MAX_JIT_MOP_FLAGS_OWN_OUTPUTMATRIX|MAX_JIT_MOP_FLAGS_OWN_JIT_MATRIX);

	// attrs & methods for getattributes, dumpout, maxjitclassaddmethods, etc
	max_jit_class_wrap_standard(max_class, jit_class, 0);

	// add our custom outputmatrix handler
	max_jit_class_addmethod_usurp_low(max_class, (method)max_jit_matrixtests_outputmatrix, "outputmatrix");

	class_addmethod(max_class, (method)max_jit_mop_assist, "assist", A_CANT, 0);	// standard matrix-operator (mop) assist fn

	class_register(CLASS_BOX, max_class);
	max_jit_matrixtests_class = max_class;
}

void max_jit_matrixtests_outputmatrix(t_max_jit_matrixtests *x)
{
	long outputmode=max_jit_mop_getoutputmode(x);
	void *mop=max_jit_obex_adornment_get(x,_jit_sym_jit_mop);
	t_jit_err err;

	if (outputmode && mop) { //always output unless output mode is none
		if (outputmode == 1) {
			if ((err=(t_jit_err)jit_object_method(
												  max_jit_obex_jitob_get(x),
												  _jit_sym_matrix_calc,
												  jit_object_method(mop,_jit_sym_getinputlist),
												  jit_object_method(mop,_jit_sym_getoutputlist))))
			{
				jit_error_code(x,err);
			}
			else {
				max_jit_mop_outputmatrix(x);
			}
		}
		else {
			max_jit_mop_outputmatrix(x);
		}
	}
}

void max_jit_matrixtests_free(t_max_jit_matrixtests *x)
{
	max_jit_mop_free(x);
	jit_object_free(max_jit_obex_jitob_get(x));
	max_jit_object_free(x);
}

void *max_jit_matrixtests_new(t_symbol *s, long argc, t_atom *argv)
{
	t_max_jit_matrixtests *x;
	void *o;

	x = (t_max_jit_matrixtests *)max_jit_object_alloc(max_jit_matrixtests_class, gensym("jit_matrixtests"));
	if (x) {
		o = jit_object_new(gensym("jit_matrixtests"));
		if (o) {
			max_jit_mop_setup_simple(x, o, argc, argv);
			max_jit_attr_args(x, argc, argv);
		}
		else {
			jit_object_error((t_object *)x, "jit.matrixtests: could not allocate object");
			object_free((t_object *)x);
			x = NULL;
		}
	}
	return (x);
}

