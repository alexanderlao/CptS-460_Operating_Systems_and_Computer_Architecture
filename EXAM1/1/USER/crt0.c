int argc;
char *argv[10];

main0 (char *s)
{
    char *token;
    int i = 0;

    printf ("main0: s = %s\n", s);

    token = strtok (s, " ");

    while (token)
    {
	argv[i] = token;

	argc++, i++;
	token = strtok (0, " ");
    }

    main (argc, argv);
}
