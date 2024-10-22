#
# Main Makefile. This is basically the same as a component makefile.
#

ifdef CONFIG_EXAMPLE_EMBEDDED_CERTS
# Certificate files. certificate.pem.crt & private.pem.key must be downloaded
# from AWS, see README for details.
COMPONENT_EMBED_TXTFILES := certs/certs/aws-root-ca.pem certs/certs/certificate.pem.crt certs/certs/private.pem.key certs/certs/ota_root.pem

# Print an error if the certificate/key files are missing
$(COMPONENT_PATH)/certs/certs/certificate.pem.crt $(COMPONENT_PATH)/certs/private.pem.key:
	@echo "Missing PEM file $@. This file identifies the ESP32 to AWS for the example, see README for details."
	exit 1
endif
