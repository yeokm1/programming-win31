/*-----------------------
   SHOWPOP.H header file
  -----------------------*/

struct
     {
     char *szAbb ;
     char *szState ;
     long lPop ;
     }
     pop [] = {
              "AL", "Alabama",             0, "AK", "Alaska",              0,
              "AZ", "Arizona",             0, "AR", "Arkansas",            0,
              "CA", "California",          0, "CO", "Colorado",            0,
              "CT", "Connecticut",         0, "DE", "Delaware",            0,
              "DC", "Dist. of Columbia",   0, "FL", "Florida",             0,
              "GA", "Georgia",             0, "HI", "Hawaii",              0,
              "ID", "Idaho",               0, "IL", "Illinois",            0,
              "IN", "Indiana",             0, "IA", "Iowa",                0,
              "KS", "Kansas",              0, "KY", "Kentucky",            0,
              "LA", "Louisiana",           0, "ME", "Maine",               0,
              "MD", "Maryland",            0, "MA", "Massachusetts",       0,
              "MI", "Michigan",            0, "MN", "Minnesota",           0,
              "MS", "Mississippi",         0, "MO", "Missouri",            0,
              "MT", "Montana",             0, "NE", "Nebraska",            0,
              "NV", "Nevada",              0, "NH", "New Hampshire",       0,
              "NJ", "New Jersey",          0, "NM", "New Mexico",          0,
              "NY", "New York",            0, "NC", "North Carolina",      0,
              "ND", "North Dakota",        0, "OH", "Ohio",                0,
              "OK", "Oklahoma",            0, "OR", "Oregon",              0,
              "PA", "Pennsylvania",        0, "RI", "Rhode Island",        0,
              "SC", "South Carolina",      0, "SD", "South Dakota",        0,
              "TN", "Tennessee",           0, "TX", "Texas",               0,
              "UT", "Utah",                0, "VT", "Vermont",             0,
              "VA", "Virginia",            0, "WA", "Washington",          0,
              "WV", "West Virginia",       0, "WI", "Wisconsin",           0,
              "WY", "Wyoming",             0, "US", "United States Total", 0
              } ;

#define NUM_STATES (sizeof (pop) / sizeof (pop [0]))
