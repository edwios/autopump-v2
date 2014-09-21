autopump-v2
===========

Automatic water pump

It is written for Spark Core to monitor and pump away excess water automatically when water level rise beyond the High Water Level threshold, and stop pumping when water level falls below the Low Water Level threshold.

It has an emergency pump activation function which can be invoked by the cloud API function "pump". It will start the water pump for 10s. After 10s, the pump will stop automatically.

If the water level does not lower to trigger the Low Water Level threshold within 15s (defined by the value of the constant called TIMEOUT), a critical level Push alert will be sent to the iPhone via PushingBox.com.
