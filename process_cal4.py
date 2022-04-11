#!/usr/bin/env python



import sys
import datetime
import copy
import re



class process_cal():
    ''' Class containing the constructor and all required functions for the assignment  '''
    
    def __init__(self, f_name):

        ''' constructor reads the file and copies it into a list. The strong is processed and a list of events is created, which is then checked for recurring events and then makes another list ''' 
        list1 = []
        with open(f_name, 'r') as f:

            for line in f:
                list1.append(line[0:len(line) - 1])

            self.event_list = self.process_string(list1)

            list2 = []
            for event in self.event_list:
                if len(event['rrule']):
                    temp = self.recurring(event)
                    if temp:
                        list2.extend(temp)

            self.event_list.extend(list2)
            

            # list comprehension to get rid of dates and times outside the range specified; replaces the list with a more
            # relevant/useful subset
            # self.event_list = [event for event in event_list if event_in_range(start_var, event, end_var)]

            # the list is sorted chronologically
            self.event_list.sort(key=self.chronological)

            # the final output
            #self.final_print(self.event_list)

    
    
    def get_events_for_day(self, given_date):
'''gets all events for the given date and formats them in the required manner '''
#        print('test')
        output_str = ""
        
        flag = False
        output_str = (given_date.strftime('%B %d, %Y (%a)\n'))
        #output_str += '\n'
        output_str += (('-' * (len(output_str)-1))+'\n')
       # print(output_str)        

        for event in self.event_list:
            if given_date.date() == event['dtstart_str'].date():      
                output_str = self.final_print(event, output_str)
                flag = True
                   #print(output_str)
        output_str = output_str[0:len(output_str)-1]            
        if flag == False: 
            return None
        #print(output_str)
        return output_str
    
    def process_string(self, str_list):
'''processes the string read from file and extracts start date, end date, summary of the event, location '''
        summary = 'SUMMARY'
        location = 'LOCATION'
        dtstart_str = 'DTSTART'
        dtend_str = 'DTEND'
        rrule_str = 'RRULE'

        date_arr = []
        time_arr = []
        rrule_arr = []
        date_arr2 = []
        time_arr2 = []
        eventList = []
        
        for i in str_list:
            if re.match(location, i):
                location_str = i[9:]
#                print(location_str)
            if re.match(summary, i):
                summary_str = i[8:]
                
            if re.match(dtstart_str, i):
                start_date = i[8:16]
                start_time = i[17:]
                start_hr = int(i[17:19])
                start_min = int(i[19:21])
                start_year = int(i[8:12])
                start_month = int(i[12:14])
                start_day = int(i[14:16])
                date_arr.extend([start_year, start_month, start_day])
                time_arr.extend([start_hr, start_min])

                # check for repetition of events
                # split the rrule list entry into another list by using the delimiter ";"
            if re.match(rrule_str, i):
                rrule_arr = i.split(";")
                rrule_arr[0] = rrule_arr[0][6:]

                # zero-padding is valid here so as the positions are known, the values for year, month, day as well as hour
                # and minute can be extracted, for both start and end times
            if re.match(dtend_str, i):
                end_date = i[6:14]
                end_time = i[15:]
                end_hr = int(i[15:17])
                end_min = int(i[17:19])
                end_year = int(i[6:10])
                end_month = int(i[10:12])
                end_day = int(i[12:14])
                date_arr2.extend([end_year, end_month, end_day])
                time_arr2.extend([end_hr, end_min])

            if re.match('END:VEVENT', i):
                # here check for the end of an event. If it has ended, a dict is initialized and the event details are
                # copied into it as key-value pairs
                eventDict = {}

                eventDict['summary'] = summary_str
                eventDict['location'] = location_str
                eventDict['dtstart_str'] = self.convert_date(date_arr)
                eventDict['timestart_str'] = self.convert_time(time_arr)
                eventDict['dtend_str'] = self.convert_date(date_arr2)
                eventDict['timeend_str'] = self.convert_time(time_arr2)
                eventDict['rrule'] = rrule_arr

                eventList.append(eventDict)
                date_arr = []
                time_arr = []
                date_arr2 = []
                time_arr2 = []
                rrule_arr = []

        date_arr.extend([start_year, start_month, start_day])
        time_arr.extend([start_hr, start_min])

        final_date = self.convert_date(date_arr)
        final_time = self.convert_time(time_arr)
        return eventList

    
    def convert_date(self, date_list):
        start_date = datetime.datetime(date_list[0], date_list[1], date_list[2])
        return start_date
    
    def convert_time(self, time_list):
        start_time = datetime.time(time_list[0], time_list[1])
        return start_time
    
    def recurring(self, event):
        rrules = event['rrule']
        freq_str = 'FREQ'
        until_str = 'UNTIL'
        until_date_arr = []
        until_time_arr = []

        for i in rrules:
            if freq_str in i:
                if 'WEEKLY' in i:
                    time_delta = datetime.timedelta(7) # 7 days
            if until_str in i:
                until_hr = int(i[15:17])
                until_min = int(i[17:19])
                until_year = int(i[6:10])
                until_month = int(i[10:12])
                until_day = int(i[12:14])


        # get the end date range
        until_date_arr.extend([until_year, until_month, until_day])
        until_time_arr.extend([until_hr, until_min])

        until_date = self.convert_date(until_date_arr)
        until_time = self.convert_time(until_time_arr)
        # print(until_date, until_time)
        # new list and dict, to store repeating events
        new_events = []
        new_event = {}
        temp_date = event['dtstart_str']
        while temp_date < until_date:
            temp_date += time_delta
            new_event = copy.deepcopy(event)
            new_event['dtstart_str'] = temp_date
            new_events.append(new_event)

        return new_events


    
    def final_print(self, event, st):
'''formatted string of the event '''
        output_str = ""
        output_str += (event['timestart_str'].strftime('%l:%M %p') + ' to ' + event['timeend_str'].strftime('%l:%M %p') + ': ' +
                  event['summary'] + ' {{' + event['location'] + '}}'+'\n')
        
        return (st + output_str)
    
    def chronological(self, event_dictionary):
'''chronological order of date and time '''
        return (event_dictionary['dtstart_str'], event_dictionary['timestart_str'])
    
    def event_in_range(self, beginning_range, ev, end_range):
'''checks if event in range or not '''
        if beginning_range <= ev['dtstart_str'] <= end_range:
            return True
        else:
            return False




