package unvr

import (
	cryptosha256 "crypto/sha256"
	"crypto/tls"
	"crypto/x509"
	"encoding/hex"
	"fmt"
	"net/http"
	"strings"
	"time"
)

// Client is a UniFi Protect API client.
type Client struct {
	baseURL string
	http    *http.Client
	apiKey  string
}

// New creates a new UNVR client.
func New(host string, port int, tlsMode, tlsFingerprint, apiKey string) (*Client, error) {
	baseURL := fmt.Sprintf("https://%s:%d/proxy/protect/integration", host, port)

	tlsConfig := &tls.Config{
		InsecureSkipVerify: tlsMode == "skip",
	}

	if tlsMode == "fingerprint" && tlsFingerprint != "" {
		tlsConfig.VerifyPeerCertificate = func(rawCerts [][]byte, verifiedChains [][]*x509.Certificate) error {
			for _, raw := range rawCerts {
				fp := hex.EncodeToString(sha256Hash(raw))
				if fp == strings.ReplaceAll(tlsFingerprint, ":", "") {
					return nil
				}
			}
			return fmt.Errorf("certificate fingerprint mismatch")
		}
	}

	return &Client{
		baseURL: baseURL,
		http: &http.Client{
			Timeout: 30 * time.Second,
			Transport: &http.Transport{
				TLSClientConfig: tlsConfig,
			},
		},
		apiKey: apiKey,
	}, nil
}

func sha256Hash(data []byte) []byte {
	h := cryptosha256.Sum256(data)
	return h[:]
}

func (c *Client) doRequest(method, path string, body interface{}) (*http.Response, error) {
	var req *http.Request
	var err error

	if body != nil {
		req, err = http.NewRequest(method, c.baseURL+path, nil)
		if err != nil {
			return nil, err
		}
	} else {
		req, err = http.NewRequest(method, c.baseURL+path, nil)
		if err != nil {
			return nil, err
		}
	}

	req.Header.Set("X-API-Key", c.apiKey)

	return c.http.Do(req)
}
