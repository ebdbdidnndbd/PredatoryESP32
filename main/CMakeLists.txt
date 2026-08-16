idf_component_register(SRCS 
    "main.c" 
    "attacks/deauth.c" 
    "attacks/beacon.c" 
    "attacks/probe_sniff.c" 
    "attacks/evil_twin.c" 
    "attacks/rickroll.c"
    "web_server.c"
    INCLUDE_DIRS "."
)
