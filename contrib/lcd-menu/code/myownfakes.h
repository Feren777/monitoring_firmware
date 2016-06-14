


typedef uint32_t timestamp_t;

typedef struct
{
  uint8_t sec;                  /* Sekunden 0-59 */
  union
  {
    uint8_t cron_fields[4];
    struct
    {
      uint8_t min;              /* Minuten 0-59 */
      uint8_t hour;             /* Stunden 0-23 */
      uint8_t day;              /* Tag des Monats 1-31 */
      uint8_t month;            /* Monat 1-12 */
    };
  };
  uint8_t dow;                  /* Tag der Woche 0-6, So-Sa */
  uint8_t year;                 /* Jahr seit 1900 */
  uint16_t yday;                /* Tag des Jahres 0-365 */
  int8_t isdst;                 /* Sommerzeit */
} clock_datetime_t;



typedef uint16_t uip_ip4addr_t[2];
typedef uint16_t uip_ip6addr_t[8];
#if UIP_CONF_IPV6
typedef uip_ip6addr_t uip_ipaddr_t;
#else /* UIP_CONF_IPV6 */
typedef uip_ip4addr_t uip_ipaddr_t;
#endif /* UIP_CONF_IPV6 */
