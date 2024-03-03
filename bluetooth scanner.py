import asyncio
from bleak import BleakScanner

async def scan():
    devices = await BleakScanner.discover(timeout=2)
    for device in devices:
        if device.name is not None:
            print(device)

asyncio.run(scan()) 