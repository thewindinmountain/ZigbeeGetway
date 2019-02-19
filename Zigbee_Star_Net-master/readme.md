# 实现功能

- AT指令的基本解析
- 主设备可以查询无线传感网中每一个终端的MAC地址（目前是所有的传感器）
- 根据MAC地址，可以从主设备发送数据到传感网中的每一个的终端

# 终端列表查询

## 查询协议

![image](F12AAD8837104C28B01F03C3C9B8A8F6)

## 响应协议

![image](D7845711E865450C93025BF4FF450C7C)

## 示例数据

终端输入

    7E 00 05 09 01 43 4C 66
                   C  L
本系统规定 CL 为查询所有的终端(Children list)

输出(这是有两个终端的情况)

    7E 00 55 88 01 43 4C 00 00 00 00 00 00 02 00 54 0E 3B 00 15 8D 00 0E 3B 00 54 0E 23 00 15 8D 00 0E 23 F0 

## 数据结构

    typedef struct
    {
        uint32 u32ExtAddrL;
        uint32 u32ExtAddrH;
        uint16 u16ShortAddr;
    } tsNodeAddr;
    
    typedef struct
    {
        /* Data related to associated end devices */
        uint32  		u32ScanChannels;
        uint16          u16Children;
        tsNodeAddr         asChild[MAX_CHILDREN];
    } tsNodeAddrTable;
    
由于上述结构满足字节对齐，因此，本应用把上面这个终端列表信息全部返回给了上位机，同时对app_childTable.c 中的API进行了扩展
    
# 数据下发

## 协议

![image](CB325D3FC2304758A3E11C6E8CBF5589)

## 示例数据

    7E 00 0A 10 00 00 54 0E 3B 00 15 8D 00 0E 3B 51 23 45 55 99
    
这里是向网络地址为0E 3B 这个终端节点发送数据，这里将会返回一个应答帧
    
    目前在数据上只是给了一个确认性数据帧，但是确认已经没问题了
    
## 下发通信过程
    
1. AT Frame     ->      Coor    ->    EndPoint        
2. EndPoint 产生应答帧
3. EndPoint ack ->      Coor    ->    Master

上述过程已经写出，为了避免对以前的代码进行大改，对msg.c中的下述函数进行了扩展

    PUBLIC void rfMsgHandle(uint8 u8DeviceType, uint64 u64RxAddr, uint16 u16RxSize, uint8 *pu8RxPayload)


# 串口编号

    数据通过调试串口，和用户串口，都可以下发AT指令和JN5168进行交互
