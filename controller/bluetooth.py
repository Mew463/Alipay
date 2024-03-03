import asyncio

from bleak import BleakScanner, BleakClient
from bleak.backends.scanner import AdvertisementData
from bleak.backends.device import BLEDevice

class BLE_UART:
    UART_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
    UART_RX_CHAR_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
    UART_TX_CHAR_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
    isConnected = False
    
    def __init__(self, peripheral_name="", address = ""):
        self._peripheral_name = peripheral_name
        self._address = address
        self._rx_queue = asyncio.Queue()
        
    async def read(self):
        try:
            msg = await self._rx_queue.get()
            return msg.decode()
        except Exception as e:
            await self.disconnect()

    async def write(self, msg): # Where msg is a String
        try:
            await self._client.write_gatt_char(self.UART_RX_CHAR_UUID, msg.encode())
        except Exception as e:
            await self.disconnect()

    async def connect(self):
        self._discovery_queue = asyncio.Queue()
        device = None
        print(f"Scanning for {self._peripheral_name}")
        async with BleakScanner(detection_callback=self._find_uart_device):
            device: BLEDevice = await self._discovery_queue.get()
        client = self._client = BleakClient(device, disconnected_callback=self._handle_disconnect)
        await client.connect()
        await client.start_notify(self.UART_TX_CHAR_UUID, self._rx_handler)    
        print(f"Connected to {self._peripheral_name}!")
        self.isConnected = True
        
    async def disconnect(self):
        print(f"Disconnected from {self._peripheral_name}")
        self.isConnected = False
        await self._client.disconnect()
        
    def _rx_handler(self, _: int, data: bytearray):
        self._rx_queue.put_nowait(data)
    
    def _find_uart_device(self, device: BLEDevice, adv: AdvertisementData):
        if device.address == self._address:
            self._discovery_queue.put_nowait(device)
        
    def _handle_disconnect(self, _: BleakClient):
        self._rx_queue.put_nowait(None)
