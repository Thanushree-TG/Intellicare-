import cv2
from pyzbar.pyzbar import decode
def QRReader():
	vs = cv2.VideoCapture(0)
	while True:
		# read the image in numpy array using cv2
		ret, img = vs.read()
		# Decode the barcode image
		detectedBarcodes = decode(img)
		d=''
		t=''
		# Traverse through all the detected barcodes in image
		for barcode in detectedBarcodes:
			# Locate the barcode position in image
			(x, y, w, h) = barcode.rect
			# Put the rectangle in image using
			# cv2 to heighlight the barcode
			cv2.rectangle(img, (x-10, y-10),
						(x + w+10, y + h+10),
						(255, 0, 0), 2)
			d = barcode.data
			t = barcode.type
		if d != "":
			d = d.decode('utf-8', 'ignore')		
		#Display the image
		cv2.imshow("Image", img)
		if cv2.waitKey(1) & 0xFF == ord('q'):
					break
		if d != "":
			break
	vs.release()
	cv2.destroyAllWindows()
	print(d)
	return d
