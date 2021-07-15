import xml.etree.ElementTree as ET


class CoordinateHelper:
    lngOri = 0
    latOri = 0
    lngUnitX = 0
    lngUnitY = 0
    latUnitX = 0 
    latUnitY = 0

    def __init__(self) -> None:
        tree = ET.parse('gpsref.xml')
        root = tree.getroot()
        for child in root: 
            if child.tag == "Origin":
                for tinyChild in child: 
                    if tinyChild.tag == "longitude":
                        self.lngOri = float(tinyChild.text)
                    elif tinyChild.tag == "latitude":
                        self.latOri = float(tinyChild.text)
                    else:
                        pass
            elif child.tag == "Metric":
                for tinyChild in child: 
                    if tinyChild.tag == "longitude_diff_in_xm":
                        self.lngUnitX = float(tinyChild.text)
                    elif tinyChild.tag == "longitude_diff_in_ym":
                        self.lngUnitY = float(tinyChild.text)
                    elif tinyChild.tag == "latitude_diff_in_xm":
                        self.latUnitX = float(tinyChild.text)
                    elif tinyChild.tag == "latitude_diff_in_ym":
                        self.latUnitY = float(tinyChild.text)
                    else:
                        pass

    def gps_to_xy(self, lng, lat):
        x = (lng-self.lngOri) / self.lngUnitX + (lat - self.latOri) / self.latUnitX
        y = (lng-self.lngOri) / self.lngUnitY + (lat - self.latOri) / self.latUnitY
        return (x,y)


a = CoordinateHelper()
print(a.lngOri)
print(a.latOri)
print(a.lngUnitX)
print(a.lngUnitY)
print(a.latUnitX)
print(a.latUnitY)
    
   


 




