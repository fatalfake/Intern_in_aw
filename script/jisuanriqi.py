import calendar
import datetime


def get_this_monday():

    today = datetime.date.today()
    oneday = datetime.timedelta(days=1)
    m1 = calendar.MONDAY

    while today.weekday() != m1:
        today -= oneday

    this_monday = today.strftime('%Y%m%d')
    return this_monday

print (get_this_monday())

def get_start_date():

    today = datetime.date.today()
    oneday = datetime.timedelta(days=1)
    m1 = calendar.MONDAY

    while today.weekday() != m1:
        today -= oneday

    start_date = today - datetime.timedelta(weeks=14)

    return start_date

print (get_start_date().strftime('%Y-%m-%d %H:%M:%S'))

