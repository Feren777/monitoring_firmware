#-----------------------------------------------------------------
# pycparser: usingcpplibc.py
#
# Shows how to use the provided 'cpp' (on Windows, substitute for
# the 'real' cpp if you're on Linux/Unix) and "fake" libc includes
# to parse a file that includes standard C headers.
#
# Copyright (C) 2008-2015, Eli Bendersky
# License: BSD
#-----------------------------------------------------------------
import sys
import pprint
import csv

import networkx as nx
import matplotlib.pyplot as plt 

# This is not required if you've installed pycparser into
# your site-packages/ with setup.py
#
sys.path.extend(['.', '..'])


from pycparser import parse_file


menutest = [
{ 'id': 0, 'title': 'ROOT', 'next': 1, 'parent': 0, 'childgroup': 0, 'nextk': 0, 'enterk': 0, 'display': 0, 'itext': "" },
{ 'id': 1, 'title': 'mi_system_info', 'next': 5, 'parent': 0, 'childgroup': 12, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_basis_info_display', 'itext': "System Info" },
{ 'id': 5, 'title': 'mi_clock_info', 'next': 6, 'parent': 0, 'childgroup': 34, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_menu_time_display', 'itext': "=[ Info CLOCK ]=====" },
{ 'id': 6, 'title': 'mi_network1_info', 'next': 7, 'parent': 0, 'childgroup': 43, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_menu_network_1_display', 'itext': "=[ Info NETWORK 1 ]=" },
{ 'id': 7, 'title': 'mi_network2_info', 'next': 1, 'parent': 0, 'childgroup': 45, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_menu_network_2_display', 'itext': "=[ Info NETWORK 2 ]=" },

{ 'id': 11, 'title': 'mi_config_set_menu', 'next': 12, 'parent': 11, 'childgroup': 11, 'nextk': 'nav_next', 'enterk': 'nav_up', 'display': 'al_list_submenus_display', 'itext': "=[ Menu AQUALOOP ]==" },
{ 'id': 12, 'title': 'mi_config_mem_set_menu', 'next': 13, 'parent': 11, 'childgroup': 20, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_list_submenus_display', 'itext': "1: Membranes config " },
{ 'id': 13, 'title': 'mi_config_sof_set_menu', 'next': 14, 'parent': 11, 'childgroup': 21, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_list_submenus_display', 'itext': "2: Suction overflow " },
{ 'id': 14, 'title': 'mi_config_slp_set_menu', 'next': 15, 'parent': 11, 'childgroup': 23, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_list_submenus_display', 'itext': "3: Sludge pump" },
{ 'id': 15, 'title': 'mi_config_c12_set_menu', 'next': 16, 'parent': 11, 'childgroup': 23, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_list_submenus_display', 'itext': "4: C1/C2 suction" },
{ 'id': 16, 'title': 'mi_config_air_set_menu', 'next': 17, 'parent': 11, 'childgroup': 27, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_list_submenus_display', 'itext': "5: AIR on/off timer " },
{ 'id': 17, 'title': 'mi_config_t1_set_menu', 'next': 18, 'parent': 11, 'childgroup': 29, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_list_submenus_display', 'itext': "6: T1 starting time " },
{ 'id': 18, 'title': 'mi_config_t2_set_menu', 'next': 19, 'parent': 11, 'childgroup': 31, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_list_submenus_display', 'itext': "7: T2 starting time " },
{ 'id': 19, 'title': 'mi_config_back_set_menu', 'next': 12, 'parent': 11, 'childgroup': 1, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_list_submenus_display', 'itext': "Return" },

{ 'id': 20, 'title': 'mi_config_set_mem', 'next': 12, 'parent': 11, 'childgroup': 12, 'nextk': 'inc_MEM', 'enterk': 'set_para', 'display': 'al_menu_change_MEM_number', 'itext': "Change MEM (number) " },
{ 'id': 21, 'title': 'mi_config_set_sof', 'next': 13, 'parent': 11, 'childgroup': 13, 'nextk': 'inc_SO', 'enterk': 'set_para', 'display': 'al_menu_change_SO_number', 'itext': "Change SO (weeks)" },
{ 'id': 22, 'title': 'mi_config_set_slpw', 'next': 14, 'parent': 11, 'childgroup': 24, 'nextk': 'inc_SLPw', 'enterk': 'set_para', 'display': 'al_menu_change_SLP_week', 'itext': "Change SLP (weeks)" },
{ 'id': 23, 'title': 'mi_config_set_slpd', 'next': 14, 'parent': 11, 'childgroup': 24, 'nextk': 'inc_SLPd', 'enterk': 'set_para', 'display': 'al_menu_change_SLP_day', 'itext': "Change SLP (days)" },
{ 'id': 24, 'title': 'mi_config_set_slpm', 'next': 14, 'parent': 11, 'childgroup': 14, 'nextk': 'inc_SLPm', 'enterk': 'set_para', 'display': 'al_menu_change_SLP_min', 'itext': "Change SLP (minutes)" },
{ 'id': 25, 'title': 'mi_config_set_c1', 'next': 15, 'parent': 11, 'childgroup': 26, 'nextk': 'inc_C1', 'enterk': 'set_para', 'display': 'al_menu_change_C1_number', 'itext': "Change C1 (cycles)" },
{ 'id': 26, 'title': 'mi_config_set_c2', 'next': 15, 'parent': 11, 'childgroup': 15, 'nextk': 'inc_C2', 'enterk': 'set_para', 'display': 'al_menu_change_C2_number', 'itext': "Change C2 (cycles)" },
{ 'id': 27, 'title': 'mi_config_set_airon', 'next': 16, 'parent': 11, 'childgroup': 28, 'nextk': 'inc_AIRon', 'enterk': 'set_para', 'display': 'al_menu_change_AIR_on', 'itext': "Change AIR ON (min) " },
{ 'id': 28, 'title': 'mi_config_set_airoff', 'next': 16, 'parent': 11, 'childgroup': 16, 'nextk': 'inc_AIRoff', 'enterk': 'set_para', 'display': 'al_menu_change_AIR_off', 'itext': "Change AIR OFF (min)" },
{ 'id': 29, 'title': 'mi_config_set_t1h', 'next': 17, 'parent': 11, 'childgroup': 30, 'nextk': 'inc_T1h', 'enterk': 'set_para', 'display': 'al_menu_change_T1_hour', 'itext': "Change T1 (hour)" },
{ 'id': 30, 'title': 'mi_config_set_t1m', 'next': 17, 'parent': 11, 'childgroup': 17, 'nextk': 'inc_T1m', 'enterk': 'set_para', 'display': 'al_menu_change_T1_min', 'itext': "Change T1 (minutes) " },
{ 'id': 31, 'title': 'mi_config_set_t2h', 'next': 18, 'parent': 11, 'childgroup': 32, 'nextk': 'inc_T2h', 'enterk': 'set_para', 'display': 'al_menu_change_T2_hour', 'itext': "Change T2 (hour)" },
{ 'id': 32, 'title': 'mi_config_set_t2m', 'next': 18, 'parent': 11, 'childgroup': 18, 'nextk': 'inc_T2m', 'enterk': 'set_para', 'display': 'al_menu_change_T2_min', 'itext': "Change T2 (minutes) " },

{ 'id': 33, 'title': 'mi_clock_set_menu', 'next': 34, 'parent': 33, 'childgroup': 33, 'nextk': 'nav_next', 'enterk': 'nav_up', 'display': 'al_list_submenus_display', 'itext': "=[ Menu CLOCK ]=====" },
{ 'id': 34, 'title': 'mi_clock_cl_set_menu', 'next': 35, 'parent': 33, 'childgroup': 37, 'nextk': 'nav_next', 'enterk': 'nav_CL', 'display': 'al_list_submenus_display', 'itext': "1: Set Time" },
{ 'id': 35, 'title': 'mi_clock_da_set_menu', 'next': 36, 'parent': 33, 'childgroup': 39, 'nextk': 'nav_next', 'enterk': 'nav_CL', 'display': 'al_list_submenus_display', 'itext': "2: Set Date" },
{ 'id': 36, 'title': 'mi_clock_back_set_menu', 'next': 34, 'parent': 33, 'childgroup': 5, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_list_submenus_display', 'itext': "Return" },

{ 'id': 37, 'title': 'mi_clock_set_thh', 'next': 34, 'parent': 33, 'childgroup': 38, 'nextk': 'inc_LThh', 'enterk': 'set_clock', 'display': 'al_menu_change_LT_hour', 'itext': "Set Time (hour)" },
{ 'id': 38, 'title': 'mi_clock_set_tmm', 'next': 34, 'parent': 33, 'childgroup': 34, 'nextk': 'inc_LTmm', 'enterk': 'set_clock', 'display': 'al_menu_change_LT_min', 'itext': "Set Time (minutes)" },
{ 'id': 39, 'title': 'mi_clock_set_tdd', 'next': 35, 'parent': 33, 'childgroup': 40, 'nextk': 'inc_LTdy', 'enterk': 'set_clock', 'display': 'al_menu_change_LT_day', 'itext': "Set Date (day)" },
{ 'id': 40, 'title': 'mi_clock_set_tmo', 'next': 35, 'parent': 33, 'childgroup': 41, 'nextk': 'inc_LTmo', 'enterk': 'set_clock', 'display': 'al_menu_change_LT_month', 'itext': "Set Date (month)" },
{ 'id': 41, 'title': 'mi_clock_set_tyr', 'next': 35, 'parent': 33, 'childgroup': 35, 'nextk': 'inc_LTyr', 'enterk': 'set_clock', 'display': 'al_menu_change_LT_year', 'itext': "Set Date (year)" },

{ 'id': 42, 'title': 'mi_network1_set_menu', 'next': 43, 'parent': 42, 'childgroup': 42, 'nextk': 'nav_next', 'enterk': 'nav_up', 'display': 'al_list_submenus_display', 'itext': "=[ Menu NETWORK ]===" },
{ 'id': 43, 'title': 'mi_network_ip_set_menu', 'next': 44, 'parent': 42, 'childgroup': 48, 'nextk': 'nav_next', 'enterk': 'nav_IPA', 'display': 'al_list_submenus_display', 'itext': "1: IP Address" },
{ 'id': 44, 'title': 'mi_network_gwy_set_menu', 'next': 45, 'parent': 42, 'childgroup': 52, 'nextk': 'nav_next', 'enterk': 'nav_GWY', 'display': 'al_list_submenus_display', 'itext': "2: GATEWAY Address" },
{ 'id': 45, 'title': 'mi_network_dns_set_menu', 'next': 46, 'parent': 42, 'childgroup': 56, 'nextk': 'nav_next', 'enterk': 'nav_DNS', 'display': 'al_list_submenus_display', 'itext': "3: DNS Address" },
{ 'id': 46, 'title': 'mi_network_ntp_set_menu', 'next': 47, 'parent': 42, 'childgroup': 60, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_list_submenus_display', 'itext': "4: NTP Address" },
{ 'id': 47, 'title': 'mi_network1_back_set_menu', 'next': 43, 'parent': 42, 'childgroup': 6, 'nextk': 'nav_next', 'enterk': 'nav_down', 'display': 'al_list_submenus_display', 'itext': "Return" },

{ 'id': 48, 'title': 'mi_network1_set_ip1', 'next': 43, 'parent': 42, 'childgroup': 49, 'nextk': 'inc_IPA1', 'enterk': 'set_IPA', 'display': 'al_menu_change_IPA_1', 'itext': "Set IP Address (A)" },
{ 'id': 49, 'title': 'mi_network1_set_ip2', 'next': 43, 'parent': 42, 'childgroup': 50, 'nextk': 'inc_IPA2', 'enterk': 'set_IPA', 'display': 'al_menu_change_IPA_2', 'itext': "Set IP Address (B)" },
{ 'id': 50, 'title': 'mi_network1_set_ip3', 'next': 43, 'parent': 42, 'childgroup': 51, 'nextk': 'inc_IPA3', 'enterk': 'set_IPA', 'display': 'al_menu_change_IPA_3', 'itext': "Set IP Address (C)" },
{ 'id': 51, 'title': 'mi_network1_set_ip4', 'next': 43, 'parent': 42, 'childgroup': 43, 'nextk': 'inc_IPA4', 'enterk': 'set_IPA', 'display': 'al_menu_change_IPA_4', 'itext': "Set IP Address (D)" },

{ 'id': 52, 'title': 'mi_network1_set_gw1', 'next': 44, 'parent': 42, 'childgroup': 53, 'nextk': 'inc_GWY1', 'enterk': 'set_GWY', 'display': 'al_menu_change_GWY_1', 'itext': "Set GWY Address (A)" },
{ 'id': 53, 'title': 'mi_network1_set_gw2', 'next': 44, 'parent': 42, 'childgroup': 54, 'nextk': 'inc_GWY2', 'enterk': 'set_GWY', 'display': 'al_menu_change_GWY_2', 'itext': "Set GWY Address (B)" },
{ 'id': 54, 'title': 'mi_network1_set_gw3', 'next': 44, 'parent': 42, 'childgroup': 55, 'nextk': 'inc_GWY3', 'enterk': 'set_GWY', 'display': 'al_menu_change_GWY_3', 'itext': "Set GWY Address (C)" },
{ 'id': 55, 'title': 'mi_network1_set_gw4', 'next': 44, 'parent': 42, 'childgroup': 44, 'nextk': 'inc_GWY4', 'enterk': 'set_GWY', 'display': 'al_menu_change_GWY_4', 'itext': "Set GWY Address (D)" },

{ 'id': 56, 'title': 'mi_network2_set_ns1', 'next': 45, 'parent': 42, 'childgroup': 57, 'nextk': 'inc_DNS1', 'enterk': 'set_DNS', 'display': 'al_menu_change_DNS_1', 'itext': "Set DNS Address (A)" },
{ 'id': 57, 'title': 'mi_network2_set_ns2', 'next': 45, 'parent': 42, 'childgroup': 58, 'nextk': 'inc_DNS2', 'enterk': 'set_DNS', 'display': 'al_menu_change_DNS_2', 'itext': "Set DNS Address (B)" },
{ 'id': 58, 'title': 'mi_network2_set_ns3', 'next': 45, 'parent': 42, 'childgroup': 59, 'nextk': 'inc_DNS3', 'enterk': 'set_DNS', 'display': 'al_menu_change_DNS_3', 'itext': "Set DNS Address (C)" },
{ 'id': 59, 'title': 'mi_network2_set_ns4', 'next': 45, 'parent': 42, 'childgroup': 45, 'nextk': 'inc_DNS4', 'enterk': 'set_DNS', 'display': 'al_menu_change_DNS_4', 'itext': "Set DNS Address (D)" },

]


menu = [{ 'id': 0, 'title': 'ROOT', 'next': '0', 'parent': 0, 'childgroup': 0, 'nextk': '0', 'enterk': '0', 'display': '0', 'itext': '0' }]

index = { 0 : "ROOT" }






def menu_to_tree(graph):
    
    # Zuerst das ROOT-Menu aufbauen (alle Items in der oberen Ebene)
    for item, key in graph.nodes(data=True):

        if key['id'] == 0: print("G 0:+-[%2d] %s" % (key['id'], key['itext']) )

        if ( (key['enterk'] == 'nav_down') and (key['parent'] == 0) ):

            #print("Edge: %d to %s" % (item, graph.node[0]['title']))
            graph.add_edge( 0, item )
            key['level'] = 1

            print(" %2d:+----[%2d] %s" % (key['id'], key['id'], key['itext']) )


    # Alle Header-Menupunkte unter die ersten Items der oberen Ebene plazieren (Menu-Titles)
    #for item, key in graph.nodes(data=True):
        # Suche nach Submenu Headern, sind mit ENTER: nav_up codiert ...
        if (key['enterk'] == 'nav_up'):

            key['level'] = 2
            #print("NAV-UP: ID = %d" % (p['id']) )
            # Diese Items müssen jetzt den oberen Ordnern zugewiesen werden:
            # Durchforsten aller Menupunkte nach Kriterium: NEXT_SCREEN(aktuell) == CHILD/GROUP(liste) und PARENT(liste) == ROOT
            for node, attr in graph.nodes(data=True):            

                # print(attr['id'], key['id'])
                # Liefert alle Submenus-Titel, die in den Info-Screens als Menupunkte verfügbar sind (erstmal nur die Titel)
                if ( (key['next'] == attr['childgroup']) and (attr['parent'] == 0)  ):
                    
                    #print("%s is in: %s" % (key['itext'], attr['itext']) )

                    graph.add_edge( item, node )
                    key['level'] = 2

                    print(" %2d:+----[%2d] %s" % (attr['id'], key['id'], key['itext']) )

    # Alle Submenu-Items unterhalb der Titel einordnen
    #for item, key in graph.nodes(data=True):
        # Suche nach Submenu items, die nicht in der oberen Ebene sind, NEXT_FUNCT == nav_next und nicht RETURN == nav_up haben
        if ( (key['nextk'] == 'nav_next') and (key['parent'] != 0) and (key['enterk'] != 'nav_up') ):

            #print("ID: %d : %s" % (key['id'], key['itext']) )

            # Diese Items müssen jetzt den oberen Ordnern zugewiesen werden, wobei der Titel schon in der Ausgabe enthalten ist:
            # Durchforsten aller Menupunkte nach Kriterium: NEXT_SCREEN(aktuell) == CHILD/GROUP(liste) und PARENT(liste) == ROOT
            for node, tops in graph.nodes(data=True):            

                # Liefert das erste Submenu-Item unterhalb des Titels. (Titel ist nicht "scrollbar", daher sind nicht alle Unterpunkte auch die Kinder...)
                if ( (key['id'] == tops['childgroup'] ) and (tops['nextk'] == 'nav_next') ):

                    #print("%s is subset of %s" % (key['itext'], tops['itext']) )
                    # key ist jetzt das erste Element unterhalb der gesuchten Oberkategorie...??

                    # Suche nach allen ITEMS die in dieser Untergruppe enthalten sind (der Titel gibt quasi vor, welche Unterpunkte dazu gehören...)
                    # Idee: Wir verfolgen diesen Punkt virtuell solange mit NEXT, bis wir auf RETUR stoßen und so die Liste voll haben...
                    for items, subs in graph.nodes(data=True):

                        # Liefert alle Unterpunkte der gerade bearbeiteten Kategorie

                        # Aktuelle Kategorie ID             ist: node (tops['id'])
                        # Aktuelles Item zur bearbeitung    ist: item (key['id'])

                        if ( (key['parent'] == subs['parent']) and (subs['nextk'] == 'nav_next') and (subs['enterk'] != 'nav_up') ):

                            #print("%s is SUBSET of %s" % (subs['itext'], tops['itext'] )  )
                            graph.add_edge( items, node )
                            subs['level'] = 3

                            print(" %2d:+------[%2d] %s" % (tops['id'], subs['id'], subs['itext']) )



    # jedes Item prüfen: Ist aktueller Name in der Spalte CHILD und steht in jenem NEXT-KEX nav_nex, dann ist aktuelles ITEM ein Untermenu jener Zeile
    #for item, key in graph.nodes(data=True):
        # Aktuell haben alle Sub-Sub-Unterpunkte keine nav_next Zuordnung (außer ROOT)
        if ( ((key['nextk'] != 'nav_next') and (key['next'] != '0')) and (key['id'] != 0) ):
            #print(key['itext'])
            # Dieses Sub-Setting Screens jetzt den Submenu Items zuordnen.... 
            for node, subs in graph.nodes(data=True):

                # Dieses p Item gehört unter das Submenu Item...
                if ( (key['next'] == subs['id']) and (subs['nextk'] == 'nav_next') ):
                    
                    #print("%d:%s below %d:%s" % (key['id'], key['itext'], subs['id'], subs['itext']) )
                    graph.add_edge( item, node )   
                    key['level'] = 4

                    print(" %2d:+--------[%2d] %s" % (subs['id'], key['id'], key['itext']) )






if __name__ == "__main__":
    if len(sys.argv) > 1:
        filename  = sys.argv[1]
    else:
        filename = 'code/aqualoop_menu.c'

'''
    ast = parse_file(filename, use_cpp=True,
        cpp_path='cpp', 
        cpp_args=r'-Ipycparser/utils/fake_libc_include')
'''





with open('code/menu.csv', 'r') as csvfile:
    csvreader = csv.reader(csvfile, delimiter=';')

    data_id = 1

    # First create the index list with the menu names
    for row in csvreader:
        # NAME;NEXT;PARENT;CHILDGROUP;NEXT;ENTER;DISPLAY;TEXT;
        # 0    1    2      3          4    5     6       7
        #index.append('id': data_id, 'menu_item': row[0])
        
        #print("'%d' : '%s'" % (data_id, row[0]) )
        index.update({data_id: row[0]}) 
        data_id += 1
    
    #pprint.pprint(index)



with open('code/menu.csv', 'r') as csvfile:
    csvreader = csv.reader(csvfile, delimiter=';')

    data_id = 1

    menu_id = 0
    child_id = 0
    parent_id = 0
    next_id = 0

    M=nx.Graph()

    M.add_node( 0, id=0, title='ROOT', next=0, parent=0, childgroup=0, nextk=0, enterk=0, display=0, itext='ROOT:', level=0 )
    
    for row in csvreader:

        #print("Processing line: %d" % data_id)

        for id, menu_name in index.items():


            if (row[1] == menu_name):
                next_id = id
            if (row[2] == menu_name):
                parent_id = id
            if (row[3] == menu_name):
                child_id = id

        #menu.append({ 'id': data_id, 'title': row[0], 'next': next_id, 'parent': parent_id, 'childgroup': child_id, 'nextk': row[4], 'enterk': row[5], 'display': row[6], 'itext': row[7] } )
        
        M.add_node( data_id, id=data_id, title=row[0], next=next_id, parent=parent_id, childgroup=child_id, nextk=row[4], enterk=row[5], display=row[6], itext=row[7], level=0 )
        
        #nxmenu.add_node(1, time='5pm')
        #print("%d { 'id': %d, 'title': '%s', 'next': %d, 'parent': %d, 'childgroup': %d, 'nextk': '%s', 'enterk': '%s', 'display': '%s', 'itext': \"%s\" 'level': 0 }"  % (data_id, data_id, row[0], next_id, parent_id, child_id, row[4], row[5], row[6], row[7]) )
            
        data_id += 1









#print(M.node[2]['itext'])
#ast.show()

menu_to_tree(M)

#tree_2 = list_to_tree(data)


# order = [ 'id', 'title', 'next', 'parent', 'childgroup', 'nextk', 'enterk', 'display', 'itext', 'sub' ]
# result = sorted(tree_1, key=lambda x:order[i[0]])
# print(result)


#pprint.pprint(tree_1)

#pprint.pprint(menu)


#pos=nx.spring_layout(M) # positions for all nodes

# nodes
#nx.draw_networkx_nodes(M,pos,node_size=700)

# edges
#nx.draw_networkx_edges(M,pos)

# labels
#nx.draw_networkx_labels(M,pos,font_size=20,font_family='sans-serif')


# write dot file to use with graphviz
# run "dot -Tpng test.dot >test.png"
nx.write_dot(M, 'test.dot')



#pos=nx.graphviz_layout(M, prog='dot')
#nx.draw(M, pos, with_labels=True, arrows=False)

#plt.axis('off')
#plt.savefig("diagram.png") # save as png
#plt.show() # display

'''
for nodes, key in M.nodes(data=True):

    if key['level'] == 0: print(" +-%s" % (key['itext']) )

    if (key['id'] != 0):
        if key['level'] == 1: print(" +--%s" % (key['itext']) )
        if key['level'] == 2: print(" +----%s" % (key['itext']) )
        if key['level'] == 3: print(" +------%s" % (key['itext']) )
        if key['level'] == 4: print(" +--------%s" % (key['itext']) )
'''
