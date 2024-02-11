#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include <string.h>
#include <stddef.h>

#include "bulgogi/inc/core.h"

typedef struct {
        PyObject_HEAD
        int number;
} CustomObject;

typedef struct bul_py_core {
        PyObject_HEAD
        /** Public */
        PyObject *py_targets;
        /** Internal */
        bul_core_s core;
} Core;

typedef struct bul_py_target {
        PyObject_HEAD
        /** Public */
        PyObject *py_deps;
        /** Internal */
        bul_target_s target;
} Target;


/** Forward Declarations*/
static PyObject *Core_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static int       Core_init(Core *self, PyObject *args, PyObject *kwds);
static void      Core_dealloc(Core *self);

static PyObject *Target_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static int       Target_init(Target *self, PyObject *args, PyObject *kwds);
static void      Target_dealloc(Target *self);

static PyObject *Custom_add_one(CustomObject *self, PyObject *Py_UNUSED(ignored));
static PyObject *Core_raw_targets(Core *self, PyObject *Py_UNUSED(ignored));
static PyObject *Core_targets(Core *self, PyObject *Py_UNUSED(ignored));
static PyObject *Core_targets(Core *self, PyObject *Py_UNUSED(ignored));

static PyTypeObject CustomType;
static PyTypeObject CoreType;
static PyTypeObject TargetType;

static PyObject *
Core_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        Core *self;

        self = (Core*) type->tp_alloc(type, 0);
        if(self == NULL) {
                return NULL;
        }

        /** Internal */
        self->core = bul_core_init();

        /** External */
        self->py_targets = PyList_New(0);

        return (PyObject*) self;
}

static int
Core_init(Core *self, PyObject *args, PyObject *kwds) {
        size_t x, y;
        FILE *file = NULL;
        char* filename = NULL;
        bul_id_t dep_id = BUL_MAX_ID;
        PyObject *py_dep = NULL;
        PyObject *py_args = NULL;
        PyObject *py_deps = NULL;
        PyObject *py_target = NULL;
        static char *kwlist[] = {"from_file", NULL};

        // TODO: Make the =from_file= optional one day.
        if(!PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &filename)) {
                return -1;
        }

        file = fopen(filename, "rb");
        if(!file) {
                return -1;
        }

        /** Internal */
        bul_core_from_file(&self->core, file);

        /** External */
        for(x = 0; x < self->core.size; x++) {
                py_args = Py_BuildValue("Is", self->core.targets[x].id, self->core.targets[x].name);
                py_target = PyObject_CallObject((PyObject*)&TargetType, py_args);

                PyList_Append(self->py_targets, py_target);

                Py_DECREF(py_args);
                Py_DECREF(py_target);
        }

        /** After all raw targets initialized */
        for(x = 0; x < self->core.size; x++) {
                py_target = PyList_GetItem(self->py_targets, x);
                if(!py_target) {
                        return -1;
                }

                py_deps = PyObject_GetAttrString(py_target, "deps");
                if(!py_deps) {
                        return -1;
                }

                for(y = 0; y < self->core.targets[x].size; y++) {
                        dep_id = self->core.targets[x].deps[y];

                        py_dep = PyList_GetItem(self->py_targets, dep_id);

                        PyList_Append(py_deps, py_dep);
                }

                Py_DECREF(py_deps);
        }

        fclose(file);

        return 0;
}

static void
Core_dealloc(Core *self) {
        bul_core_free(&self->core);
        Py_DECREF(self->py_targets);
        Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject *
Target_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        Target *self;

        self = (Target*) type->tp_alloc(type, 0);
        if(self == NULL) {
                return NULL;
        }

        self->target.id = BUL_MAX_ID;
        self->target.name = NULL;
        self->target.size = 0;
        self->target.deps = NULL;

        return (PyObject*) self;
}

static int
Target_init(Target *self, PyObject *args, PyObject *kwds) {
        char *name = NULL;
        bul_id_t id = BUL_MAX_ID;
        static char *kwlist[] = {"id", "name", NULL};

        if(!PyArg_ParseTupleAndKeywords(args, kwds, "Is", kwlist, &id, &name)) {
                return -1;
        }

        if(id == BUL_MAX_ID) {
                return -1;
        }

        if(!name) {
                return -1;
        }

        /** Internal */
        self->target = bul_target_init(id, name);

        /** Public */
        self->py_deps = PyList_New(0);

        return 0;
}

static void
Target_dealloc(Target *self) {
        if(self->target.name) {
                free(self->target.name);
                free(self->target.deps);
        }
        Py_DECREF(self->py_deps);
        Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject *
Custom_add_one(CustomObject *self, PyObject *Py_UNUSED(ignored)) {
        self->number += 1;

        Py_RETURN_NONE;
}

static PyObject *
Core_raw_targets(Core *self, PyObject *Py_UNUSED(ignored)) {
        Py_INCREF(self->py_targets);
        return self->py_targets;
}

static PyObject *
Core_targets(Core *self, PyObject *Py_UNUSED(ignored)) {
        int done;
        size_t x;
        PyObject *name = NULL;
        PyObject *targets = NULL;
        PyObject *document = NULL;
        PyObject *next_item = NULL;

        done = 0;
        for(x = 0; x < self->core.size; x++) {
                next_item = PyList_GetItem(self->py_targets, x);

                name = PyObject_GetAttrString(next_item, "name");
                if(!name) {
                        return NULL;
                }

                if(strcmp(PyUnicode_DATA(name), "DOCUMENT") == 0) {
                        document = next_item;
                        done = 1;
                /* Ensures `name` is freed */
                }

                Py_DECREF(name);

                if(done) {
                        break;
                }
        }
        /* loop should typically find DOCUMENT in the first iteration */

        if(document) {
                targets = PyObject_GetAttrString(document, "deps");
                if(!targets) {
                        return NULL;
                }

                return targets;
        } else {
                Py_RETURN_NONE;
        }
}

static PyMemberDef Custom_members[] = {
        {"number", T_INT, offsetof(CustomObject, number), 0, "custom number"},
        {NULL},
};

static PyMethodDef Custom_methods[] = {
        {"add_one", (PyCFunction) Custom_add_one, METH_NOARGS, "Adds one to the number field"},
        {NULL},
};

static PyMethodDef Core_methods[] = {
        {"raw_targets", (PyCFunction) Core_raw_targets, METH_NOARGS, "Retrieves the list of *all* core targets."},
        {"targets", (PyCFunction) Core_targets, METH_NOARGS, "Retrieves the list of targets at the DOCUMENT root."},
        {NULL},
};

static PyMemberDef Target_members[] = {
        {"id", T_INT, offsetof(Target, target.id), 0, "Target ID"},
        {"size", T_INT, offsetof(Target, target.size), 0, "Target Size (Number of deps)"},
        {"name", T_STRING, offsetof(Target, target.name), 0, "Target Name"},
        {"deps", T_OBJECT, offsetof(Target, py_deps), 0, "Target Dependencies"},
        {NULL},
};

static PyTypeObject CustomType = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "bul.Custom",
        .tp_doc  = PyDoc_STR("A custom object"),
        .tp_basicsize = sizeof(CustomObject),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_new = PyType_GenericNew,
        .tp_members = Custom_members,
        .tp_methods = Custom_methods,
};

static PyTypeObject CoreType = {
        .ob_base      = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name      = "bul.Core",
        .tp_doc       = PyDoc_STR("Bulgogi Core Object"),
        .tp_basicsize = sizeof(Core),
        .tp_itemsize  = 0,
        .tp_flags     = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new       = Core_new,
        .tp_init      = (initproc) Core_init,
        .tp_dealloc   = (destructor) Core_dealloc,
        .tp_methods   = Core_methods,
};

static PyTypeObject TargetType = {
        .ob_base      = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name      = "bul.Target",
        .tp_doc       = PyDoc_STR("Bulgogi Target Object"),
        .tp_basicsize = sizeof(Target),
        .tp_itemsize  = 0,
        .tp_flags     = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new       = Target_new,
        .tp_init      = (initproc) Target_init,
        .tp_dealloc   = (destructor) Target_dealloc,
        .tp_members   = Target_members,
};

static PyObject *bul_py_system(PyObject *self, PyObject *args) {
        const char *command;
        int sts;

        if(!PyArg_ParseTuple(args, "s", &command)) {
                return NULL;
        }
        sts = system(command);
        return PyLong_FromLong(sts);
}

static PyObject *bul_py_core_from_file(PyObject *self, PyObject *args) {
        PyObject   *core_py   = NULL;
        PyObject   *dep_list  = NULL;
        PyObject   *dep_str   = NULL;

        size_t     x, y;
        bul_core_s core;
        bul_id_t   dep_id     = BUL_MAX_ID;

        FILE       *file      = NULL;
        const char *filename  = NULL;

        if(!PyArg_ParseTuple(args, "s", &filename)) {
                return NULL;
        }

        if(!(file = fopen(filename, "rb"))) {
                return NULL;
        }

        core = bul_core_init();

        bul_core_from_file(&core, file);

        fclose(file);

        core_py = PyDict_New();

        for(x = 0; x < core.size; x++) {
                dep_list = PyList_New(core.targets[x].size);

                for(y = 0; y < core.targets[x].size; y++) {
                        dep_id = core.targets[x].deps[y];

                        dep_str = PyUnicode_FromString(core.targets[dep_id].name);

                        PyList_SetItem(dep_list, y, dep_str);
                }

                PyDict_SetItemString(core_py, core.targets[x].name, dep_list);

                Py_DecRef(dep_list);
        }

        return core_py;
}

static PyMethodDef BulMethods[] = {
        {"system", bul_py_system, METH_VARARGS, "Execute a shell command."},
        {"core_from_file", bul_py_core_from_file, METH_VARARGS, "Initializes a core from a YAML file."},
        {NULL, NULL, 0, NULL},
};

static struct PyModuleDef bulmodule = {
        PyModuleDef_HEAD_INIT,
        "bulgogi",
        NULL, /* module doc */
        -1,
        BulMethods,
};

PyMODINIT_FUNC PyInit_bulgogi(void) {
        PyObject *m = NULL;

        if(PyType_Ready(&CustomType) < 0) {
                return NULL;
        }

        if(PyType_Ready(&CoreType) < 0) {
                return NULL;
        }

        if(PyType_Ready(&TargetType) < 0) {
                return NULL;
        }

        m = PyModule_Create(&bulmodule);
        if(m == NULL) {
                return NULL;
        }

        Py_INCREF(&CustomType);
        if(PyModule_AddObject(m, "Custom", (PyObject*) &CustomType) < 0) {
                Py_DECREF(&CustomType);
                Py_DECREF(m);
                return NULL;
        }

        Py_INCREF(&CoreType);
        if(PyModule_AddObject(m, "Core", (PyObject*) &CoreType) < 0) {
                Py_DECREF(&CoreType);
                Py_DECREF(m);
                return NULL;
        }

        Py_INCREF(&TargetType);
        if(PyModule_AddObject(m, "Target", (PyObject*) &TargetType) < 0) {
                Py_DECREF(&TargetType);
                Py_DECREF(m);
                return NULL;
        }

        return m;
}
