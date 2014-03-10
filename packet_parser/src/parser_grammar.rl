%%{
    # write your name
    machine parser_grammar;

    alphtype unsigned char;

    startflag = print{3};
    endflag = print{3};

    main := (
        (startflag @startflag_received)
        (endflag @endflag_received)
    );
}%%
