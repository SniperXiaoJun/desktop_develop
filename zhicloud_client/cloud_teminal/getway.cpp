#include "getway.h"

// if_name like "ath0", "eth0". Notice: call this function
// need root privilege.
// return value:
// -1 -- error , details can check errno
// 1 -- interface link up
// 0 -- interface link down.
int get_netlink_status(const char *if_name)
{
    int skfd;
    struct ifreq ifr;
    struct ethtool_value edata;
    edata.cmd = ETHTOOL_GLINK;
    edata.data = 0;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, if_name, sizeof(ifr.ifr_name) - 1);
    ifr.ifr_data = (char *) &edata;
    if (( skfd = socket( AF_INET, SOCK_DGRAM, 0 )) == 0)
        return -1;
    if(ioctl( skfd, SIOCETHTOOL, &ifr ) == -1)
    {
        close(skfd);
        return -1;
    }
    close(skfd);
    return edata.data;
}

int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{
    struct nlmsghdr *nlHdr;
    int readLen = 0, msgLen = 0;
    do{
        //�յ��ں˵�Ӧ��
        if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0)
        {
            perror("SOCK READ: ");
//��ǩ��Linuxϵͳ C++ ��ȡ����ӿڣ��Լ���������IP�� ����,����,IP
            return -1;
        }
        nlHdr = (struct nlmsghdr *)bufPtr;
        //���header�Ƿ���Ч
        if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
        {
            perror("Error in recieved packet");
            return -1;
        }
//��ǩ��Linuxϵͳ C++ ��ȡ����ӿڣ��Լ���������IP�� ����,����,IP
        if(nlHdr->nlmsg_type == NLMSG_DONE)
        {
            break;
        }
        else
        {
            bufPtr += readLen;

            msgLen += readLen;
        }
        if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)
//��ǩ��Linuxϵͳ C++ ��ȡ����ӿڣ��Լ���������IP�� ����,����,IP
        {
            break;
        }
    } while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));
    return msgLen;
}
 
//�������ص�·����Ϣ
void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway, char *ifName)
{
    struct rtmsg *rtMsg;
    struct rtattr *rtAttr;
//��ǩ��Linuxϵͳ C++ ��ȡ����ӿڣ��Լ���������IP�� ����,����,IP
    int rtLen;
    char *tempBuf = NULL;
    struct in_addr dst;
    struct in_addr gate;
 
    tempBuf = (char *)malloc(100);
    rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);
    // If the route is not for AF_INET or does not belong to main routing table
    //then return.
    if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
        return;
//����ǩ��Linuxϵͳ C++ ��ȡ����ӿڣ��Լ���������IP�� ����,����,IP
 
    rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
    rtLen = RTM_PAYLOAD(nlHdr);
    for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen)){
        switch(rtAttr->rta_type) {
        case RTA_OIF:
            if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
            break;
        case RTA_GATEWAY:
//����ǩ��Linuxϵͳ C++ ��ȡ����ӿڣ��Լ���������IP�� ����,����,IP
            rtInfo->gateWay = *(u_int *)RTA_DATA(rtAttr);
            break;
        case RTA_PREFSRC:
            rtInfo->srcAddr = *(u_int *)RTA_DATA(rtAttr);
            break;
        case RTA_DST:
            rtInfo->dstAddr = *(u_int *)RTA_DATA(rtAttr);
            break;
//��ǩ��Linuxϵͳ C++ ��ȡ����ӿڣ��Լ���������IP�� ����,����,IP
        }
    }
    dst.s_addr = rtInfo->dstAddr;
    if (strstr((char *)inet_ntoa(dst), "0.0.0.0"))
    {
        sprintf(ifName, "%s", rtInfo->ifName);
        //printf("oif:%s",rtInfo->ifName);
        gate.s_addr = rtInfo->gateWay;
        sprintf(gateway, "%s", (char *)inet_ntoa(gate));
//��ǩ��Linuxϵͳ C++ ��ȡ����ӿڣ��Լ���������IP�� ����,����,IP
        //printf("%sn",gateway);
        gate.s_addr = rtInfo->srcAddr;
        //printf("src:%sn",(char *)inet_ntoa(gate));

        gate.s_addr = rtInfo->dstAddr;
        //printf("dst:%sn",(char *)inet_ntoa(gate));
    }
    free(tempBuf);
    return;
}
 
int get_gateway(char *gateway, char *ifName)
//��ǩ��Linuxϵͳ C++ ��ȡ����ӿڣ��Լ���������IP�� ����,����,IP
{
    struct nlmsghdr *nlMsg;
    struct rtmsg *rtMsg;
    struct route_info *rtInfo;
    char msgBuf[BUFSIZE];
 
    int sock, len, msgSeq = 0;
 
    if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
    {
        perror("Socket Creation: ");
        return -1;
    }
    memset(msgBuf, 0, BUFSIZE);
 

    nlMsg = (struct nlmsghdr *)msgBuf;
    rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);
 

    nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
    nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .
 
    nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
    nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
    nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.
 

    if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0){
        printf("Write To Socket Failed��n");
        return -1;
    }
 

    if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0) {
        printf("Read From Socket Failed��n");
        return -1;
    }
    rtInfo = (struct route_info *)malloc(sizeof(struct route_info));
    for(;NLMSG_OK(nlMsg,len);nlMsg = NLMSG_NEXT(nlMsg,len)){
        memset(rtInfo, 0, sizeof(struct route_info));
        parseRoutes(nlMsg, rtInfo, gateway, ifName);
    }
    free(rtInfo);
    close(sock);
    return 0;
}

bool getmaskAddress(QString &ip, QString &netmask,QString &mac)
{
	QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
	int count = list.size();
	int i = 0;
	QNetworkInterface interfaceq;
	for (i=0; i < count; i++)
	{
			interfaceq = list[i];
			qDebug() << "Device:" << interfaceq.name();//�豸����
			if(interfaceq.name() == QString("eth0"))
			{
			  mac = interfaceq.hardwareAddress();
			}
			qDebug() << "HardwareAddress:" << interfaceq.hardwareAddress();//��ȡӲ����ַ
			QList<QNetworkAddressEntry> entryList = interfaceq.addressEntries();//��ȡip��ַ����������͹㲥��ַ
			for (int ii = 0; ii < entryList.size(); ii++)
			{
					QNetworkAddressEntry entry = entryList[ii];
					QHostAddress address = entry.ip();
					if(QNetworkInterface().allAddresses().size() < 3)
					{
					printf("size < 3\n");
					return false;
					}
					if (address.toString() == QNetworkInterface().allAddresses().at(2).toString()) {

						if (QString("eth0") == interfaceq.name())
						{
							ip= entry.ip().toString();//��ip
							netmask = entry.netmask().toString();//������
							entry.broadcast().toString();//������
						}
						else
						{
							ip = QString("");
							netmask = QString("");
						}
						return true;
					}

			}
	}
	if (count == i)
		return false;
}


 #if 0
int main()
{
    char buff[256], ifName[12];
    get_gateway(buff, ifName);
    printf("interface:%s\ngetway: %s\n",ifName, buff);
    return 0;
}
#endif
