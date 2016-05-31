#  ----------------------------------------------------------------
# Copyright 2016 Cisco Systems
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ------------------------------------------------------------------
""" codec_service.py

   The Encoder/Decoder Service class.

"""
from .service import Service
from meta_service import MetaService
import logging


class CodecService(Service):
    """ Codec Service to encode and decode XML, JSON and other protocol payloads """
    def __init__(self):
        self.service_logger = logging.getLogger(__name__)

    def encode(self, encoder, entity):
        assert encoder is not None, 'Encoder should be valid object'
        MetaService.normalize_meta([], entity)
        payload = encoder._encode(entity)
        self.service_logger.info('Encoding operation completed\n')
        return payload

    def decode(self, decoder, payload):
        assert decoder is not None, 'Decoder should be valid object'
        entity = decoder._decode(payload)
        self.service_logger.info('Decoding operation completed\n')
        return entity
