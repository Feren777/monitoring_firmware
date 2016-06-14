## To set up environment:

~$ virtualenv .
~$ source bin/activate

## Install modules

~$ pip3 install pycparser
~$ pip3 install netowrkx
~$ pip3 install matplotlib (takes long time! >10 min)

now run code with:

~$ python3 parser.py

or interactive mode:

~$ exec(open("./parse.py").read())


# write dot file to use with graphviz
# run "dot -Tpng test.dot >test.png"




# Zeigt alle Nodes, mit denen der Node verbunden ist:
iterativ:
for nodes in nx.all_neighbors(M,node):
	print(nx.get_node_attributes(M, 'id')[nodes])

nativ:
M.neighbors(node)

>>> exec(open("./parse.py").read())
G 0:+-[ 0] ROOT:
  1:+----[ 1] System Info
  2:+----[ 2] =[ TIMERS T1/T1 ]===
  3:+----[ 3] =[ CYCLES C1/C2 ]===
  4:+----[ 4] =[ MONITORING ]=====
  5:+----[ 5] =[ Info CLOCK ]=====
  6:+----[ 6] =[ Info NETWORK 1 ]=
  7:+----[ 7] =[ Info NETWORK 2 ]=
  9:+--------[ 7] =[ Info NETWORK 2 ]=
  8:+----[ 8] =[ TEST MODE ]======
  9:+----[ 9] Configuration
 10:+----[10] Current Sensors
  1:+----[11] =[ Menu AQUALOOP ]==
  9:+----[11] =[ Menu AQUALOOP ]==
  1:+------[12] 1: Membranes config 
  1:+------[13] 2: Suction overflow 
  1:+------[14] 3: Sludge pump
  1:+------[15] 4: C1/C2 suction
  1:+------[16] 5: AIR on/off timer 
  1:+------[17] 6: T1 starting time 
  1:+------[18] 7: T2 starting time 
  1:+------[19] Return
  9:+------[12] 1: Membranes config 
  9:+------[13] 2: Suction overflow 
  9:+------[14] 3: Sludge pump
  9:+------[15] 4: C1/C2 suction
  9:+------[16] 5: AIR on/off timer 
  9:+------[17] 6: T1 starting time 
  9:+------[18] 7: T2 starting time 
  9:+------[19] Return
 12:+--------[20] Change MEM (number) 
 13:+--------[21] Change SO (weeks)
 14:+--------[22] Change SLP (weeks)
 14:+--------[23] Change SLP (days)
 14:+--------[24] Change SLP (minutes)
 15:+--------[25] Change C1 (cycles)
 15:+--------[26] Change C2 (cycles)
 16:+--------[27] Change AIR ON (min) 
 16:+--------[28] Change AIR OFF (min)
 17:+--------[29] Change T1 (hour)
 17:+--------[30] Change T1 (minutes) 
 18:+--------[31] Change T2 (hour)
 18:+--------[32] Change T2 (minutes) 
  5:+----[33] =[ Menu CLOCK ]=====
  5:+------[34] 1: Set Time
  5:+------[35] 2: Set Date
  5:+------[36] Return
 42:+------[34] 1: Set Time
 42:+------[35] 2: Set Date
 42:+------[36] Return
 43:+------[34] 1: Set Time
 43:+------[35] 2: Set Date
 43:+------[36] Return
 44:+------[34] 1: Set Time
 44:+------[35] 2: Set Date
 44:+------[36] Return
 45:+------[34] 1: Set Time
 45:+------[35] 2: Set Date
 45:+------[36] Return
 46:+------[34] 1: Set Time
 46:+------[35] 2: Set Date
 46:+------[36] Return
 47:+------[34] 1: Set Time
 47:+------[35] 2: Set Date
 47:+------[36] Return
 34:+--------[37] Set Time (hour)
 34:+--------[38] Set Time (minutes)
 35:+--------[39] Set Date (day)
 35:+--------[40] Set Date (month)
 35:+--------[41] Set Date (year)
  6:+----[42] =[ Menu NETWORK ]===
  6:+------[43] 1: IP Address
  6:+------[44] 2: GATEWAY Address
  6:+------[45] 3: DNS Address
  6:+------[46] 4: NTP Address
  6:+------[47] Return
 43:+--------[48] Set IP Address (A)
 43:+--------[49] Set IP Address (B)
 43:+--------[50] Set IP Address (C)
 43:+--------[51] Set IP Address (D)
 44:+--------[52] Set GWY Address (A)
 44:+--------[53] Set GWY Address (B)
 44:+--------[54] Set GWY Address (C)
 44:+--------[55] Set GWY Address (D)
 45:+--------[56] Set DNS Address (A)
 45:+--------[57] Set DNS Address (B)
 45:+--------[58] Set DNS Address (C)
 45:+--------[59] Set DNS Address (D)
 46:+--------[60] Set NTP Address
 46:+--------[61] Timer Settings
 46:+--------[62] Cycle Settings
 46:+--------[63] Test Modes
>>> M.successors(43)
[6]
>>> M.predecessors(43)




RegEx menu.csv:

[^\S\r\n]{2,} whitespace expt. return
















Ideen automatische Menu-Struktur Generierung für LaTeX



- C-Code ausschneiden

- Variablennamen mit statischem Inhalt für Manual zusammenstellen
- Funktions-Returns ebenfalls mit statischem Inhalt versehen
- hd44780_goto(row, col) so nachbasteln, daß der Text in jew. Zeile und Spalte eingefügt wird
- hd44780_puts("NEXT:Next  ENTER:Set"); // Statischer Content

- lcdm_write_function(lcdm_get_current_menu()->text);
- ...
- lcdm_navigate(MENU_CHILD) und ähnliche Funktionen für Menustruktur nachbasteln




- MENU_ITEM als CSV einlesen und als PHP Array nachbasteln:

/*
 ## Settings MENU-LEVEL ##
         NAME                       NEXT                       PARENT               CHILD/GROUP            NEXT        ENTER     DISPLAY                     TEXT
*/                                                                                                                                                         //12345678901234567890
MENU_ITEM(mi_config_set_menu,       mi_config_mem_set_menu,    mi_config_set_menu,  mi_config_set_menu,    nav_next,   nav_up,   al_list_submenus_display,  "=[ Menu AQUALOOP ]==");
MENU_ITEM(mi_config_mem_set_menu,   mi_config_sof_set_menu,    mi_config_set_menu,  mi_config_set_mem,     nav_next,   nav_down, al_list_submenus_display,  "1: Membranes config ");
MENU_ITEM(mi_config_sof_set_menu,   mi_config_slp_set_menu,    mi_config_set_menu,  mi_config_set_sof,     nav_next,   nav_down, al_list_submenus_display,  "2: Suction overflow ");
MENU_ITEM(mi_config_slp_set_menu,   mi_config_c12_set_menu,    mi_config_set_menu,  mi_config_set_slpd,    nav_next,   nav_down, al_list_submenus_display,  "3: Sludge pump      ");
MENU_ITEM(mi_config_c12_set_menu,   mi_config_air_set_menu,    mi_config_set_menu,  mi_config_set_c1 ,     nav_next,   nav_down, al_list_submenus_display,  "4: C1/C2 suction    ");
MENU_ITEM(mi_config_air_set_menu,   mi_config_t1_set_menu,     mi_config_set_menu,  mi_config_set_airon,   nav_next,   nav_down, al_list_submenus_display,  "5: AIR on/off timer ");
MENU_ITEM(mi_config_t1_set_menu,    mi_config_t2_set_menu,     mi_config_set_menu,  mi_config_set_t1h,     nav_next,   nav_down, al_list_submenus_display,  "6: T1 starting time ");
MENU_ITEM(mi_config_t2_set_menu,    mi_config_back_set_menu,   mi_config_set_menu,  mi_config_set_t2h,     nav_next,   nav_down, al_list_submenus_display,  "7: T2 starting time ");
MENU_ITEM(mi_config_back_set_menu,  mi_config_mem_set_menu,    mi_config_set_menu,  mi_system_info,        nav_next,   nav_down, al_list_submenus_display,  "Return              ");

                                                                                                                                                            //12345678901234567890
MENU_ITEM(mi_config_set_mem,        mi_config_mem_set_menu,    mi_config_set_menu,  mi_config_mem_set_menu, inc_MEM,    set_para, al_menu_change_MEM_number, "Change MEM (number) ");
MENU_ITEM(mi_config_set_sof,        mi_config_sof_set_menu,    mi_config_set_menu,  mi_config_sof_set_menu, inc_SO,     set_para, al_menu_change_SO_number,  "Change SO (weeks)   ");
MENU_ITEM(mi_config_set_slpw,       mi_config_slp_set_menu,    mi_config_set_menu,  mi_config_set_slpm,     inc_SLPw,   set_para, al_menu_change_SLP_week,   "Change SLP (weeks)  ");
MENU_ITEM(mi_config_set_slpd,       mi_config_slp_set_menu,    mi_config_set_menu,  mi_config_set_slpm,     inc_SLPd,   set_para, al_menu_change_SLP_day,    "Change SLP (days)   ");
MENU_ITEM(mi_config_set_slpm,       mi_config_slp_set_menu,    mi_config_set_menu,  mi_config_slp_set_menu, inc_SLPm,   set_para, al_menu_change_SLP_min,    "Change SLP (minutes)");
MENU_ITEM(mi_config_set_c1,         mi_config_c12_set_menu,    mi_config_set_menu,  mi_config_set_c2,       inc_C1,     set_para, al_menu_change_C1_number,  "Change C1 (cycles)  ");
MENU_ITEM(mi_config_set_c2,         mi_config_c12_set_menu,    mi_config_set_menu,  mi_config_c12_set_menu, inc_C2,     set_para, al_menu_change_C2_number,  "Change C2 (cycles)  ");
MENU_ITEM(mi_config_set_airon,      mi_config_air_set_menu,    mi_config_set_menu,  mi_config_set_airoff,   inc_AIRon,  set_para, al_menu_change_AIR_on,     "Change AIR ON (min) ");
MENU_ITEM(mi_config_set_airoff,     mi_config_air_set_menu,    mi_config_set_menu,  mi_config_air_set_menu, inc_AIRoff, set_para, al_menu_change_AIR_off,    "Change AIR OFF (min)");
MENU_ITEM(mi_config_set_t1h,        mi_config_t1_set_menu,     mi_config_set_menu,  mi_config_set_t1m,      inc_T1h,    set_para, al_menu_change_T1_hour,    "Change T1 (hour)    ");
MENU_ITEM(mi_config_set_t1m,        mi_config_t1_set_menu,     mi_config_set_menu,  mi_config_t1_set_menu,  inc_T1m,    set_para, al_menu_change_T1_min,     "Change T1 (minutes) ");
MENU_ITEM(mi_config_set_t2h,        mi_config_t2_set_menu,     mi_config_set_menu,  mi_config_set_t2m,      inc_T2h,    set_para, al_menu_change_T2_hour,    "Change T2 (hour)    ");
MENU_ITEM(mi_config_set_t2m,        mi_config_t2_set_menu,     mi_config_set_menu,  mi_config_t2_set_menu,  inc_T2m,    set_para, al_menu_change_T2_min,     "Change T2 (minutes) ");



Alle Kinder:                         SELECT * FROM items WHERE name="mi_config_set_menu" AND parent="mi_config_set_menu" AND next="nav_down";

Alle Einstellungen eines Settings:   SELECT * FROM items WHERE name="mi_config_slp_set_menu" AND next="mi_config_slp_set_menu" AND enter="set_para";



array(
    'id' => 1,
    'id_parent' => 0,
    'name' => 'mi_config_set_menu',
    'next' => 'mi_config_mem_set_menu', 
    'parent' => 'mi_config_set_menu',
    'child-group' => 'mi_config_set_menu',
    'next' => 'nav_next',
    'enter' => 'nav_down',
    'display' => 'al_list_submenus_display',
    'text' => '=[ Menu AQUALOOP ]==',

    'children' => array(

        'id' => 2,
        'id_parent' => 1,
        'name' => 'mi_config_mem_set_menu',
        'children' => array(

            'id' => 3,
            'id_parent' => 2,
            'name' => 'mi_config_set_mem'
        );

        'id' => 3,
        'id_parent' => 1,
        'name' => 'mi_config_sof_set_menu',
        'children' => array(

            'id' => 4,
            'id_parent' => 3,
            'name' => 'mi_config_set_slpw';

            'id' => 5,
            'id_parent' => 3,
            'name' => 'mi_config_set_slpd'
        )
    )
)

- Aus dem PHP Array wird nun die Display Funktion verwendet, um daraus den Display-Inhalt herzustellen

