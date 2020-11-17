# SCOUTING
40MHz scouting CMS

## Control boards:

- main.py : python interface to get/set (r/rw) properties

            Arguments:
            
            -- board (-b):    choose ["board0", "board1", ...]
            -- property (-p): choose ["property0", "property1", ...]
            -- action (-a):   choose ["set", "get"]
            -- value (-v):    optional new state for property
