from dataclasses import dataclass
from typing import Optional
from abc import ABCMeta, abstractmethod


@dataclass
class TransceiveConfiguration:
    """Defines settings used for NFC communication"""
    type: str
    crc: int = True
    bits: int = 8
    bitrate: int = 106
    timeout: float = None
    # Output power as a percentage of maximum supported by the reader
    power: float = 100

    def replace(self, **kwargs):
        """Return a new instance with specific values replaced by name."""
        return self.__class__(**{
            "type": self.type,
            "crc": self.crc,
            "bits": self.bits,
            "bitrate": self.bitrate,
            "timeout": self.timeout,
            "power": self.power,
            **kwargs
        })


CARRIER = 13.56e6
A_TIMEOUT = (1236 + 384) / CARRIER
CONFIGURATION_A_LONG = TransceiveConfiguration(
    type="A", crc=True, bits=8, timeout=A_TIMEOUT
)


class ReaderTag(metaclass=ABCMeta):
    """Describes a generic target which implements ISODEP protocol"""

    @abstractmethod
    def transact(self, command_apdus, response_apdus):
        """Sends command_apdus and verifies reception of matching response_apdus
        """


class Reader(metaclass=ABCMeta):
    """Describes a generic NFC reader which can be used for running tests"""

    @abstractmethod
    def poll_a(self) -> Optional[ReaderTag]:
        """Attempts to perform target discovery by issuing Type A WUP/REQ
        and performing anticollision in case one is detected.
        Returns a tag object if one was found, None otherwise
        """

    @abstractmethod
    def poll_b(self, *, afi=0x00) -> Optional[ReaderTag]:
        """Attempts to perform target discovery by issuing Type B WUP/REQ
        and performing anticollision in case one is detected.
        Returns a tag object if one was found, None otherwise
        """

    @abstractmethod
    def send_broadcast(
        self,
        data: bytes, *,
        configuration: TransceiveConfiguration = CONFIGURATION_A_LONG
    ):
        """Broadcasts a custom data frame into the RF field.
        Does not require an active target to be detected to do that.
        By default, uses 'Long A' frame configuration, which can be overridden.
        """

    @abstractmethod
    def mute(self):
        """Disables the RF field generated by the reader"""

    @abstractmethod
    def unmute(self):
        """Enables the RF field generated by the reader"""

    @abstractmethod
    def reset(self):
        """Auxiliary function to reset reader to starting conditions"""

    def reset_buffers(self):
        """Forwards a call to .reset() for compatibility reasons"""
        self.reset()
