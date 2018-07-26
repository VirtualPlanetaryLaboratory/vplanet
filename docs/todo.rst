TODO
====

A list of all the to-dos scattered about the repo.


To create a **to-do** item anywhere in the *docs*:

    .. code-block:: rest

        .. todo::

            **@username**: To-do message here, which can
            span multiple lines. Mind the indentations;
            copy this example exactly to be safe.


To create a **to-do** anywhere in the *code*, you need
to explicitly escape the reStructureText:

    .. code-block:: C

        /**
        Function that does something.
        @param something The something we're gonna do

        \rst

            .. todo::

                **@username**: Make this function actually
                do something!

        \endrst

        */
        void DoSomething(int *something) {
            do_something_else(something);
        };


.. todolist::
